#include <stdio.h>
#include <unistd.h>
#include <error.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#define BUFFSIZE 1024
/*
Please make sure you understand host.c
*/

int main(void){

    FILE *fp;
    fp = fopen("./datalist.txt","r");
    int unread;
    char buf[BUFFSIZE]={};
    char ch;
   
    printf("HTTP/1.1 200 OK\r\n");
    printf("Content-Type: text/html; charset=UTF-8\r\n\r\n");
    // output to stdout
    printf("<html><head><link rel=\"icon\" href=\"data:;base64,=\"><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n");
    printf("<title>DB data</title>\n");
    
    printf("<body>");
    printf("<h>Click to view file</h><br>\n");
    while((fscanf(fp,"%s",buf)!=EOF)){
        printf("<button onclick=\"location.href=getdomain('getfile='+'%s')\" type=\"button\">%s</button><br>\n",buf,buf);
    }
    printf("<script>\n");
    printf("function getlink(){var nowlink = window.location.href; var link = nowlink.substring(0,nowlink.search(\"view\")); return link+'insert';}\n");
    printf("</script>\n");
    printf("<script>\n");
    printf("function getdomain(s){var nowlink = window.location.href; var link = nowlink.substring(0,nowlink.search(\"view\")); return link+s}\n");    
    printf("</script>\n");
    printf("</body></html>\n");
    printf("\n" );
    fclose(fp);
    exit(1);
}