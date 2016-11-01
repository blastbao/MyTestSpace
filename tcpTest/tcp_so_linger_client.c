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

#define SERV_ADDR "127.0.0.1"
#define SERV_PORT 9877
#define MAX_LINE 128

int main(int argc, char** argv)    
{    
    struct sockaddr_in serverAdd;
    bzero(&serverAdd, sizeof(serverAdd));
    serverAdd.sin_family = AF_INET;
    serverAdd.sin_addr.s_addr = inet_addr(SERV_ADDR);
    serverAdd.sin_port = htons(SERV_PORT);
    
    int connfd = socket(AF_INET, SOCK_STREAM, 0);
    
    int connResult = connect(connfd, (struct sockaddr *)&serverAdd, sizeof(serverAdd));
    if (connResult < 0) {
        printf("连接失败\n");
        close(connfd);
        return -1;
    }
    
    ssize_t writeLen;
    char sendMsg[5000] = {0};

    int i;
    for (i = 0 ; i<sizeof(sendMsg); i++) {
        sendMsg[i] = 'a';
    }
    if (1) {
        //write操作返回成功仅仅能说明数据已经发送到套接字的发送缓冲区，不能代表对端已经成功收到数据。
        //如果server端恰巧使用close(connfd)关掉已连接套接字，那么client在这里write成功的数据就会被server所丢弃，而client也永远不会知道。
        writeLen = write(connfd, sendMsg, sizeof(sendMsg));
        if (writeLen < 0) {
            printf("发送失败\n");
            return -1;
        }
        else
        {
            printf("发送成功 writeLen = %zd\n",writeLen);
        }
        //close的默认返回成功也仅仅是成功发出了一个FIN分节，也不代表对端已经确认。
        close(connfd);
    }
    return 0;
}



