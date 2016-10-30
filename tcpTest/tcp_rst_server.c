#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <errno.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
//server.c  
#define SERV_PORT 9877
#define WAIT_COUNT 5
#define READ_SIZE 257
int main(int argc, char** argv)    
{    
    int listen_fd, real_fd;    
    struct sockaddr_in listen_addr, client_addr;    
    socklen_t len = sizeof(struct sockaddr_in);    
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);    
    if(listen_fd == -1)    
    {    
        perror("socket failed   ");    
        return -1;    
    }    
    bzero(&listen_addr,sizeof(listen_addr));    
    listen_addr.sin_family = AF_INET;    
    listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);    
    listen_addr.sin_port = htons(SERV_PORT);    
    bind(listen_fd,(struct sockaddr *)&listen_addr, len);    
    listen(listen_fd, WAIT_COUNT);    
    while(1)    
    {    
        real_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &len);    
        if(real_fd == -1)    
        {    
            perror("accpet fail  ");    
            return -1;    
        }    
        if(fork() == 0)    
        {    
            close(listen_fd);    
            char pcContent[4096];  
            read(real_fd,pcContent,4096);  
            close(real_fd);    
            exit(0);                
        }    
        close(real_fd);    
    }       
    return 0;    
}  