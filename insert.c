#include <stdio.h>
#include <unistd.h>
#include <error.h>
#include <stdlib.h>
#include <sys/ioctl.h>

int main(){
	char form[1000]={};

	int unread;
    char *buf;
    char ch;
    FILE *fout;
    fout = fopen("./db.txt","a");

  // wait for stdin
   	while(unread<1){
        if(ioctl(STDIN_FILENO,FIONREAD,&unread)){
            perror("ioctl");
            exit(EXIT_FAILURE);
        }
    }
    buf = (char*)malloc(sizeof(char)*(unread+1));
    

    char save[200];
    // read from stdin fd
    read(STDIN_FILENO,buf,unread);
    int i=0;
    while(buf[i]){
    	if(buf[i]=='?'){
    		while(buf[i]!='='){
    			i++;
    		}
    		i++;
    		while(buf[i]){
    			fprintf(fout, "%c", buf[i]);
    			i++;
    		}
    		fprintf(fout, "%c",'\n' );
    		break;
    		
    	}
    	i++;
    }

    printf("HTTP/1.1 200 OK\r\n");
    printf("Content-Type: text/html; charset=UTF-8\r\n\r\n");
    // output to stdout
    printf("<html><head><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n");
    printf("<title>insert form</title>\n");
    printf("<body>");

    printf("<form method=\"get\">\n");
    printf("<p>data: <input type=\"text\" name=\"data\" /></p>\n");
    printf("<input type=\"submit\" value=\"Submit\" />\n");

    printf("<button onclick=\"location.href=getlink()\" type=\"button\">view</button>\n");
    printf("</form>\n");
    
    printf("<script>\n");
    printf("function getlink(){var nowlink = window.location.href; var link = nowlink.substring(0,nowlink.search(\"insert\")); return link+'view';}\n");
    printf("</script>\n");

    printf("</body></html>\n");
 	printf("\n" );
    fclose(fout);
    return 0;
}