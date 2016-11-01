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

int main(int argc, const char * argv[])
{

    struct sockaddr_in serverAdd;
    struct sockaddr_in clientAdd;
    
    bzero(&serverAdd, sizeof(serverAdd));
    serverAdd.sin_family = AF_INET;
    serverAdd.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAdd.sin_port = htons(SERV_PORT);
    
    socklen_t clientAddrLen;
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    int yes = 1;
    setsockopt(listenfd,SOL_SOCKET, SO_REUSEADDR,(void *)&yes, sizeof(yes));
    
    if (listenfd < 0) {
        printf("创建socket失败\n");
        return -1;
    }
    
    int bindResult = bind(listenfd, (struct sockaddr *)&serverAdd, sizeof(serverAdd));
    if (bindResult < 0) {
        printf("绑定端口失败\n");
        close(listenfd);
        return -1;
    }
    
    listen(listenfd, 20);
    
    int connfd;
    unsigned char recvMsg[246988];
    unsigned long long totalSize = 0;
    
    clientAddrLen = sizeof(clientAdd);
    connfd = accept(listenfd,(struct sockaddr *)&clientAdd,&clientAddrLen);
    if (connfd < 0) {
        printf("连接失败\n");
        return -1;
    }else{
        //这里我们用于测试，只接收一个连接
        close(listenfd);
    }
    
    //说明：通过抓包可以看出客户端的5000字节的数据都成功发送到服务端被server ack了)，服务器里面一秒的sleep休眠就是为了接收完这些数据，
    //关闭连接close(connfd)以后再来通过read读取connfd的接收缓冲区的数据，会读取失败，说明已经connfd套接字被内核丢弃了，而客户端的数据是已经成功写到发送缓冲区，
    //即使关闭连接，发送缓冲区的数据不会被丢弃，会正常发送到服务器，发送完成以后，接着发送终止连接的第一个FIN分节；
    sleep(1);
    close(connfd);
    while (1)
    {
        ssize_t readLen = read(connfd, recvMsg, sizeof(recvMsg));
        if (readLen < 0) {
            printf("读取失败\n");
            break;
        }
        else if (readLen == 0) {
            printf("读取完成 totalSize = %llu\n",totalSize);
            break;
        }
        else
        {
            totalSize += readLen;
            printf("readLen:%ld\n",readLen);
        }
    }
    return 0;
}
