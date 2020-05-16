#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>   //inet_addr
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#define DEBUG
#define BUFSIZE 8096

#define CA_FILE                "./CA/cacert.pem"
#define SERVER_KEY             "./server/key.pem"
#define SERVER_CERT            "./server/cert.pem"

#include <iostream>
using namespace std;

void setCA(SSL_CTX *ctx){
	// SSL_VERIFY_PEER = verify both  
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, 0);

	if (SSL_CTX_load_verify_locations(ctx, CA_FILE, 0) != 1) {
		SSL_CTX_free(ctx);
		printf("Failed to load CA file %s", CA_FILE);
	}
	//load server cert
	if (SSL_CTX_use_certificate_file(ctx, SERVER_CERT, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stdout);
		exit(1);
	}else{
		puts("load SERVER_CERT success");
	}
	// load server private key
	if (SSL_CTX_use_PrivateKey_file(ctx, SERVER_KEY, SSL_FILETYPE_PEM) <= 0) {
		printf("load private key fail.\n");
		ERR_print_errors_fp(stdout);
		exit(1);
	}else{
		puts("load server private key success");
	}
	// check if cert and private key is ok
	if (!SSL_CTX_check_private_key(ctx)) {
		ERR_print_errors_fp(stdout);
		exit(1);
	}else{
		puts("cert and privateKey ok!");
	}
}

string analyze(string str){
	int h;
	int s_end;
	string domain;
	string data;
	//GET /aaa HTTP/1.1
   	if(str.find("GET")==0){
   		h = str.find('/');
   		s_end = str.find("HTTP");
   		domain = str.assign(str.begin()+h+1,str.begin()+s_end-1);

   		cout<<domain<<"*"<<"\n";
   		return domain;
   	}
   return "";
}



int main(int argc , char *argv[]){

	int cgiInput[2];
	int cgiOutput[2];
	int status;
	
	pid_t cpid;
	char cb;
	int i;
	static char message[BUFSIZE+1]={};

	if(pipe(cgiInput)<0){
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	if(pipe(cgiOutput)<0){
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	// socket
	int socket_desc , client_sock , c ;
	// ipv4 struct
	struct sockaddr_in server , client;
	char client_message[BUFSIZE+1]={};

	//Create socket

	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(atoi(argv[1]));

	printf("port is %d\n", atoi(argv[1]));
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");

	listen(socket_desc , 3);
	//SSL init
    SSL_library_init();
    SSL_CTX *ctx = SSL_CTX_new (TLSv1_2_server_method()); 
    //set ca about
	setCA(ctx);
	SSL * ssl;
	while(1){
			//Listen
			printf("%s\n","listening" );

			if ( (client_sock = accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c)) == -1 ){
				perror("accept");
				exit(1);
			}
			if(pipe(cgiInput)<0){
				perror("pipe");
				exit(EXIT_FAILURE);
			}
			printf("%s\n","Got a client!" );
			ssl  = SSL_new(ctx);
			SSL_set_fd(ssl, client_sock);
			if (SSL_accept(ssl) == -1) {
				perror("accept");
				ERR_print_errors_fp(stderr);  
				close(client_sock);
				break;
			}
			
			cpid = fork();	
			printf("%s\n", "go fork!");

			/*parent process*/
			if(cpid > 0){ 			
				printf("this is parent process\n");
				//close unused fd
				close(cgiOutput[1]);
				close(cgiInput[0]);

				waitpid(cpid, &status, 0);
				puts("child wait end");
				
				// receive the message from the  CGI program
				i=0;
				memset(message,0,BUFSIZE);
				while (read(cgiOutput[0], &cb, 1) > 0){
					// output the message to terminal
					message[i++]=cb;
					//write(STDOUT_FILENO, &cb, 1);
				}
				SSL_write(ssl,message,sizeof(message));
				//send(client_sock,message,sizeof(message),0);
				puts("what i send\n");
				puts(message);
				
				// connection finish
				close(cgiOutput[0]);
				close(cgiInput[1]);
				SSL_shutdown(ssl);
				SSL_free(ssl);
				printf("%s\n","parent finished" );
			}/*child process*/
			else if(cpid == 0){	
				printf("this is child process\n");
				
				memset(client_message,0,BUFSIZE+1);
				/*long ret;

				ret = read(client_sock,client_message,BUFSIZE); 

				if (ret>0&&ret<BUFSIZE)
        			client_message[ret] = 0;
    			else
        			client_message[0] = 0;*/
				//printf("child gets %s\nlength is %d\n", client_message,strlen(client_message));
				SSL_read (ssl, client_message, sizeof(client_message));

				string cm;
        		cm = client_message;
        			
				cout<<"child gets:\n"<<cm<<"\n";
				string result = analyze(cm);

				cout<<"result is:"<<result<<"\n";

				//char *cstr = new char[result.length() + 1];
				//strcpy(cstr, result.c_str());
				// do stuff		
				write(cgiInput[1], result.c_str(), result.size()*sizeof(char));
				//delete [] cstr;
				//close unused fd
				close(cgiInput[1]);
				close(cgiOutput[0]);

				
				//redirect the output from stdout to cgiOutput
				dup2(cgiOutput[1],STDOUT_FILENO);


				//redirect the input from stdin to cgiInput
				dup2(cgiInput[0], STDIN_FILENO); 

				//after redirect we don't need the old fd 
				close(cgiInput[0]);
				close(cgiOutput[1]);

				
				if(result==("view")){
					execlp("./view.cgi","./view.cgi",NULL);
				}else if(result.find("insert")!=result.npos){
					execlp("./insert.cgi","./insert.cgi",NULL);		
				}else if(result == "favicon.ico"){
					exit(0);
				}
				exit(0);
			}
		 
	}
	close(socket_desc);
}
