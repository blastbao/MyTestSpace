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
        return -1 ;
    }else{
        printf("连接成功\n");
    }
    
    ssize_t writeLen;
    char sendMsg[15] = {0};
    unsigned long long totalReadSize = 0;
    unsigned long long totalWriteSize = 0;


    if (1) {
        writeLen = write(connfd, sendMsg, sizeof(sendMsg));
        if (writeLen < 0) {
            printf("发送失败 errno = %d\n",errno);
            return -1;
        }else{
            totalWriteSize += writeLen;
            printf("发送成功 totalWriteSize = %lld\n",totalWriteSize);
        }
        //关闭连接的写这一半，client进程不能再对connfd套接字调用任何写操作；
        //server进程仍可从套接字接收数据，套接字发送缓冲区中的内容被发送到server端；
        shutdown(connfd,SHUT_WR);  
        //再调用shutdown关闭写一半之后，如果继续write会导致程序直接退出。
        //writeLen = write(connfd, sendMsg, sizeof(sendMsg));

        /*Debug 检查关闭写一半之后还能否接受数据
        unsigned char recvMsg[246988];
        ssize_t readLen = read(connfd, recvMsg, sizeof(recvMsg));
        if (readLen < 0) {
            printf("读取失败 errno = %d\n",errno);
            close(connfd);
            return -1;
        }else if (readLen == 0) {
            printf("读取完成 totalReadSize = %llu\n",totalReadSize);
            close(connfd);
            return -1;
        }else{
            recvMsg[readLen] = 0;
            printf("读取数据 ：%s\n",recvMsg);
        }
        //debug end*/
        sleep(20); 
    }
    return 0;
}