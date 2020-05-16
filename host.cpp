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
#define DEBUG
#define BUFSIZE 8096
#include <iostream>
using namespace std;

string analyze(string str){
	int h;
	int d;
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
	int socket_desc , client_sock , c , read_size;
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

	
	/* Creates a new process to execute cgi program */
	

	/*		1> cgiInput  >0	
	host                      CGI
			0< cgiOutput <1				*/
	

	while(1){
			//Listen
			listen(socket_desc , 3);
			printf("%s\n","listening" );
			//client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);		
			
			if ( (client_sock = accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c)) == -1 ){
				perror("accept");
				exit(1);
			}
			if(pipe(cgiInput)<0){
				perror("pipe");
				exit(EXIT_FAILURE);
			}
			if(pipe(cgiOutput)<0){
				perror("pipe");
				exit(EXIT_FAILURE);
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
				puts("===============1\n");
				memset(message,0,BUFSIZE);
				puts(message);
				puts("===============2\n");
				// receive the message from the  CGI program
				i=0;
				while (read(cgiOutput[0], &cb, 1) > 0){
					// output the message to terminal
					message[i++]=cb;
					//write(STDOUT_FILENO, &cb, 1);

				}
				send(client_sock,message,sizeof(message),0);
				puts("what i send\n");
				puts(message);
				
				// connection finish
				close(cgiOutput[0]);
				close(cgiInput[1]);
				
				
				printf("%s\n","parent finished" );
				
			}/*child process*/
			else if(cpid == 0){	
				printf("this is child process\n");
				
				memset(message,0,BUFSIZE+1);
				long ret;

				ret = read(client_sock,client_message,BUFSIZE); 

				if (ret>0&&ret<BUFSIZE)
        			client_message[ret] = 0;
    			else
        			client_message[0] = 0;
				//printf("child gets %s\nlength is %d\n", client_message,strlen(client_message));

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
					
				}
				exit(0);
			}
		 
	}
	close(socket_desc);
}
