#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>

#include <string.h>
#include <stdlib.h>
#include <errno.h>
//#include <Attr_API.h>

int test_udp_svr(const char* ip, unsigned short port){
    int udp_svr = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (-1==udp_svr){
        printf("Failed to create socket.socket error:%d %s\n", errno, strerror(errno));
        return -1;
    }
    //
    int buffer_size = 1024*64;
    setsockopt(udp_svr, SOL_SOCKET, SO_RCVBUF, (char*)&buffer_size, sizeof(int));
    //
    struct timeval st;
    st.tv_sec  = 1;
    st.tv_usec = 0;
    setsockopt(udp_svr, SOL_SOCKET, SO_RCVTIMEO, (char*)&st, sizeof(struct timeval));
    //
    struct sockaddr_in addr;
    memset(&(addr), 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
    
    int ret = bind(udp_svr, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if (-1 == ret){
        printf("bind error:%d %s\n", errno, strerror(errno));
        close(udp_svr);
        return -1;
    }
    //
    unsigned long int dgram_count = 0;
    unsigned long long int bytes_count = 0;
    char buf[1024*64];
    struct sockaddr_in remote_addr;
    socklen_t addr_len = sizeof(struct sockaddr);
    for (;;){
        //收包（不阻塞）
        long int recv_len = recvfrom(udp_svr, buf, sizeof(buf), 0, (struct sockaddr*)&remote_addr, &addr_len);
        if (recv_len<=0){
            continue;
        }
        printf("client send data:%ld\n",recv_len);
        //统计包数+字节数
        dgram_count++;
        printf("total count of udp packet:%ld\n",dgram_count);
        bytes_count += recv_len;
        printf("total count of recieved bytes :%lld\n",bytes_count);
        //回包
        long int send_len = sendto(udp_svr, buf, recv_len, 0, (struct sockaddr*)&remote_addr, sizeof(struct sockaddr));
        if (send_len < recv_len){
            continue;
        }
        //Attr_API(2380784, recv_len);      //接收字节数
        //Attr_API(2380785, 1);             //接收次数
    }
    return 0;
}

int main(int argc, char* argv[]){
    /*
    if (argc<3){
        printf("usage:%s <ip> <port>\n", argv[0]);
        return 0;
    }
    const char* ip = argv[1];
    unsigned short port = strtoul(argv[2], NULL, 10);
    if (port<1000){
        printf("port max bigger 1000\n");
        return 0;
    }*/
    const char* ip = "127.0.0.1";
    const int port = 9877; 
    test_udp_svr(ip, port);
    return 1;
}

/*
g++ -o udp_svr.o -c udp_echo_server.cpp -g -Wall -Werror -I"/data/home/yaaf_proj/workspace/yaaf-extlib/libattrapi/"
g++ -o udp_svr udp_svr.o "/data/home/yaaf_proj/workspace/build-yaaf-extlib/libattrapi/libattrapi.a"
*/
