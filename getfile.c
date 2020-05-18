#include <stdio.h>
#include <unistd.h>
#include <error.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#define MAXBUFF 1024

int main(){
    int unread,i;
    char *buf;
    char ch;
    FILE *fp;
  // wait for stdin
    while(unread<1){
        if(ioctl(STDIN_FILENO,FIONREAD,&unread)){
            perror("ioctl");
            exit(EXIT_FAILURE);
        }
    }
    buf = (char*)malloc(sizeof(char)*(unread+1));
    char save[MAXBUFF]={};
    // read from stdin fd
    read(STDIN_FILENO,buf,unread);
    printf("HTTP/1.1 200 OK\r\n");
    printf("Content-Type: text/html; charset=UTF-8\r\n\r\n");
    // output to stdout
    printf("<html><head><link rel=\"icon\" href=\"data:;base64,=\"><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n");
    printf("<title>*%s*</title>\n",buf);
    printf("<body>\n");
    printf("<p>Go back to view</p>");
    printf("<button onclick=\"location.href=getlink()\" type=\"button\">view</button>\n");
    printf("<p>File Content:</p>");
    char filep[MAXBUFF]="./data/";
    strcat(filep,buf); 
    if((fp=fopen(filep,"r"))==NULL){
          printf("open file error!!\n");
          exit(0);
    } 
    rewind(fp);
    while(fgets(save,200,fp)!=NULL){
        printf("<p>%s</p>\n",save);
        memset(save,0,MAXBUFF);
    }
    
    printf("<script>\n");
    printf("function getlink(){var nowlink = window.location.href; var link = nowlink.substring(0,nowlink.search(\"getfile\")); return link+'view';}\n");
    printf("</script>\n");
    printf("</body></html>\n");
    printf("\n" );
    fclose(fp);
    exit(1);
}