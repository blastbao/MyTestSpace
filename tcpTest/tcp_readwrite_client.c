#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
//client.c  
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

    //设置为不开启negle算法——nodelay,这样不会合并小于mss的包。
    int enable = 1;
    if (setsockopt(connfd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable)) == -1)
    {
        printf("error when setsockopt TCP_NODELAY: %s\n", strerror(errno));
        return -1;
    }

    int connResult = connect(connfd, (struct sockaddr *)&serverAdd, sizeof(serverAdd));
    if (connResult < 0) {
        printf("连接失败，errno ＝ %d\n",errno);
        close(connfd);
        return -1 ;
    }else{
        printf("连接成功\n");
    }
    
    ssize_t writeLen;
    char sendMsg[2] = {0};
    int count = 0;

    //最多循环write 5次,每次246988个字节的数据
    while (1)
    {
        if (count == 5) {
            exit(0);
        }
        /* 这里调用write时会阻塞，是因为内核从该应用进程的缓冲区中复制所有数据到所写套接字connfd的发送缓冲区；
         * 而由于服务器server还未处理完数据，没有回复ACK确认给客户端，客户端的套接字发送缓冲区暂时要保留数据，
         * 要是connfd的发送缓冲区满了，write就会休眠等待，直到有可用的空间把要写的数据全部写完才返回；
         */
        writeLen = write(connfd, sendMsg, sizeof(sendMsg));
        if (writeLen < 0) {
            printf("发送失败\n");
            close(connfd);
            return -1;
        }else{
            printf("发送成功\n");
        }
        count++;
    }
    return 0;
}  