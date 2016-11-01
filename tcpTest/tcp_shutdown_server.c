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
    setsockopt(listenfd,SOL_SOCKET, SO_REUSEADDR, (void *)&yes, sizeof(yes));
    
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
    unsigned long long totalReadSize = 0;
    unsigned long long totalWriteSize = 0;
    clientAddrLen = sizeof(clientAdd);
    connfd = accept(listenfd,(struct sockaddr *)&clientAdd,&clientAddrLen);
    if (connfd < 0) {
        printf("连接失败\n");
        return -1;
    }else{
        printf("连接成功\n");
        //这里我们用于测试，只接收一个连接，所以把监听的fd关闭，不再接受accept新的连接。
        close(listenfd);
    }
    
    //server进程不能再对connfd套接字调用任何读操作，套接字接收缓冲区中所有数据被丢弃，server再接收到的任何数据由TCP丢弃。
    //client进程往connfd的读写不受影响，但是因为server已经拒接任何数据，client写入的数据都被丢弃，
    //所以client端的write能成功执行，但是并不能可靠交付给server应用程序，所以server会返回RST断开连接。
    //如果客户端忽略这个RST（client通过read返回0可以判断出RST）再往server进行write操作，会引发SigPipe异常并默认导致client程序终止。
    ///shutdown(connfd,SHUT_RD);

    while (1) {
        
        ssize_t readLen = read(connfd, recvMsg, sizeof(recvMsg));
        if (readLen < 0) {
            printf("读取失败 errno = %d\n",errno);
            close(connfd);
            return -1;
        }
        else if (readLen == 0) {
            //因为server端读关闭，套接字接收缓冲区的数据直接被丢弃掉了，所以read()直接返回0，可以验证一下。
            printf("读取完成 totalReadSize = %llu\n",totalReadSize);
            close(connfd);
            return -1;
        }
        else
        {
            totalReadSize += readLen;
            printf("readLen:%ld\n",readLen);

            /*Debug 检查对面关闭写一半之后还能否接收数据,所以发送一些数据让client接收一下。
            unsigned char testReply[] = {'h','e','l','l','o'};
            ssize_t writeLen = write(connfd, testReply, sizeof(testReply));
            if (writeLen < 0) {
                printf("发送失败 errno = %d\n",errno);
                return -1;
            }else{
                totalWriteSize += writeLen;
                printf("发送成功 totalWriteSize = %lld\n",totalWriteSize);
            }
            //Debug end*/
        }

    }
    
    return 0;
}
