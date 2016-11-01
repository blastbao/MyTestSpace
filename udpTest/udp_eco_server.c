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

#define SERV_PORT 9877
#define WAIT_COUNT 5
#define READ_SIZE 257
int main(int argc, const char * argv[])
{

    int socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (-1 == socketFileDescriptor) {
        printf("Failed to create socket.");
        return -1;
    }
    
    struct sockaddr_in socketParameters;
    memset(&(socketParameters), 0, sizeof(socketParameters));
    socketParameters.sin_family = AF_INET;
    socketParameters.sin_addr.s_addr = htonl(INADDR_ANY);
    socketParameters.sin_port = htons(SERVERPORT);
    
    int ret = bind(socketFileDescriptor, (struct sockaddr *) &socketParameters, sizeof(socketParameters));
    if (-1 == ret) {
        close(socketFileDescriptor);
        return -1;
    }
    
    char recvMsg[30000] = {0};
    char replyMmsg[20] = "I am server~";
    int count = 0;
    
    while (1) {
		//sleep(1);
        struct sockaddr_in clientAddress;
        socklen_t address_len = sizeof(clientAddress);;
        long byteNum = recvfrom(socketFileDescriptor,recvMsg,sizeof(recvMsg),0,(struct sockaddr *) &clientAddress,&address_len);
        
        if (byteNum>0) {
            count++;
            printf("count is:%d\n",count);
            printf("client said:%ld\n",byteNum);
        }
        
        if (byteNum>0) {
            ssize_t result = sendto(socketFileDescriptor, replyMmsg, sizeof(replyMmsg), 0, (struct sockaddr *) &clientAddress,sizeof(clientAddress));
            
            if (result > 0) {
                printf(" >> send success  ");
                
            }
            else {
                printf(" >> send fail ");
            }
        }
    }
    close(socketFileDescriptor);    
    return 0;
}