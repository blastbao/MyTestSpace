#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define SERV_IP "127.0.0.1"
#define SERV_PORT 9877
#define MAX_LINE 128

int main(int argc, const char * argv[])
{
    int socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (-1 == socketFileDescriptor) {
        printf("Failed to create socket.");
        return -1;
    }
    
    struct sockaddr_in socketParameters;
    memset(&socketParameters, 0, sizeof(socketParameters));
    socketParameters.sin_family = AF_INET;
    socketParameters.sin_addr.s_addr = inet_addr(SERV_IP);
    socketParameters.sin_port = htons(SERV_PORT);
    
    int conn = connect(socketFileDescriptor, (struct sockaddr*)&socketParameters, sizeof(socketParameters));
    if (conn < 0) {
        printf("Failed to connect socket.");
        return -1;
    }
    
    while (1) {
        
        sleep(1);
        char buffer[20];
        char sendMsg[200] = {0};
        for (int i = 0; i<sizeof(sendMsg); i++) {
            sendMsg[i] = 'u';
        }
        
        ssize_t result = sendto(socketFileDescriptor, sendMsg, sizeof(sendMsg), 0, NULL,0);
        
        if (result > 0) {
            printf(" >> send success\n  ");
        } else {
            printf(" >> send fail\n ");
        }
        
        ssize_t rresult = recvfrom(socketFileDescriptor, buffer, sizeof(buffer), 0, NULL, NULL);
        if (rresult == -1){
            printf("recvfrom fail\n"); 
        }else if(rresult > 0){
            printf("server said:%s\n",buffer);
        }

    }
    
    close(socketFileDescriptor);
    return 0;
    
}

