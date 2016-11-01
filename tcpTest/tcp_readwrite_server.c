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
#include <netinet/in.h>
#include <netinet/tcp.h>
//server.c  
#define SERV_PORT 9877
#define WAIT_COUNT 5
#define READ_SIZE 257

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
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (void *)&yes, sizeof(yes));
    
    if (listenfd < 0) {
        printf("创建socket失败\n");
        close(listenfd);
        return -1;
    }
    
    int bindResult = bind(listenfd, (struct sockaddr *)&serverAdd, sizeof(serverAdd));
    if (bindResult < 0) {
        close(listenfd);
        printf("绑定端口失败，errno ＝ %d\n",errno);
        return -1;
    }else{
        printf("绑定端口成功\n");
    }
    
    listen(listenfd, 20);  
    int connfd;
    clientAddrLen = sizeof(clientAdd);
    connfd = accept(listenfd,(struct sockaddr *)&clientAdd,&clientAddrLen);
    if (connfd < 0) {
        printf("连接失败\n");
        close(listenfd);
        return -1;
    }else{
        printf("连接成功\n");
        
        //获取默认接收缓冲区大小
        int rcvbuf_len;
        socklen_t len = sizeof(rcvbuf_len);
        if( getsockopt( connfd, SOL_SOCKET, SO_RCVBUF, (void *)&rcvbuf_len, &len ) < 0 ){
            perror("getsockopt: ");
        }
        printf("the recv buf len: %d\n", rcvbuf_len );
    }
    
    unsigned char recvMsg[246988];
    ssize_t totalLen = 0;
    
    while (1)
    {
        //TCP在处理交互数据流采用了Delayed Ack机制以及Nagle算法来减少小分组数目。

        //这里设置为不开启快速应答(TCP_QUICKACK),这样会延迟确认数据包。
        //ack其实并不会对网络性能有太大的影响，DELAY_ACK能减少带宽浪费、快速ack能及时通知，意义也就仅限于此了。
        //在协议性能需要优化时，有丢包的情况下，需要考虑启用快速ack，因为这样可以及时通知发送方丢包，避免滑动窗口停等，提升吞吐率。
        //
        //其实仅有延迟确认机制，是不会导致请求延迟的（初以为是必须等到ACK包发出去，recv系统调用才会返回，实际上并不是）。
        //一般来说，只有当DELAY_ACK机制与Nagle算法或拥塞控制（慢启动或拥塞避免）混合作用时，才可能会导致时耗增长。
        int unable = 0;
        if (setsockopt(connfd, IPPROTO_TCP, TCP_QUICKACK, &unable, sizeof(unable)) == -1)

        //这里睡眠是为了等待client发送的数据被接收完。若屏蔽掉sleep(1)代码,会发现read()每次读到的数据长度是没有规律的。
        sleep(1);
        //这里read方法读取到的数据长度readLen和套接字接收缓冲区的长度无关。
        ssize_t readLen = read(connfd, recvMsg, sizeof(recvMsg));
        printf("readLen:%ld\n",readLen);
        if (readLen < 0) {
            printf("读取失败\n");
            return -1;
        }else if (readLen == 0) {
            printf("读取完成-len = %ld\n",totalLen);
            close(listenfd);
            return 0;
        }else{
            totalLen += readLen;
        }
    }
    close(connfd);
    return 0;
}