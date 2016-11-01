/* raw_udp.c
 * Raw UDP packet sender implementation
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include "raw_udp.h"

static char errmsg[256];

/* 
 * Pseudo header needed for udp header checksum calculation 
 */
struct pseudo_header
{
    uint32_t source_address;
    uint32_t dest_address;
    uint8_t placeholder;
    uint8_t protocol;
    uint16_t udp_length;
};

/*
 *  Generic checksum calculation function
 */
static unsigned short csum(unsigned short *ptr,int nbytes) 
{
    register long sum;
    unsigned short oddbyte;
    register short answer;
 
    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }

    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }

    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;

    return(answer);
}

/*
 * Raw udp socket creation
 */
static int raw_udpsock_create()
{
    int s = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if(s == -1){
        snprintf(errmsg, sizeof(errmsg), "Failed to create raw socket: %s", strerror(errno));
        return -1;
    }
    // set non-blocking
    int flag = fcntl(s, F_GETFL, 0);
    if(flag>=0)
        fcntl(s, F_SETFL, flag|O_NONBLOCK|O_NDELAY);

    return s;
}

/*
 * The sending function
 * Arguments:
 *  buf      - the content of a UDP packet
 *  buf_len  - the length of buf
 *  src_addr - the sending address(the sin_addr should be equal to the output interface's IP address)
 *  dst_addr - the destination address
 * Returns:
 *  >=0 - successful, the number of bytes sent
 *  < 0 - failed, please call raw_udp_errorstr() to get the reason for it
 */
int raw_udp_send(char *buf, int buf_len, struct sockaddr_in *src_addr, struct sockaddr_in *dst_addr)
{
    static int s = -1;
    if(s == -1)
        s = raw_udpsock_create();
    if(s == -1)
        return -1;

    // datagram to represent the packet
    // UDP数据报最大值不能超过65535,去掉报文头部剩余部分用来存储数据。
    static char datagram[65535-sizeof(struct iphdr)+sizeof(struct pseudo_header)];

    struct pseudo_header *psh = (struct pseudo_header *)datagram;
    struct udphdr *udph = (struct udphdr *)(datagram+sizeof(struct pseudo_header));
    char *data = datagram+sizeof(struct pseudo_header)+sizeof(struct udphdr);

    //UDP header
    udph->source = src_addr->sin_port;
    udph->dest = dst_addr->sin_port;
    udph->len = htons(sizeof(struct udphdr)+buf_len); //udp header size
    udph->check = 0; //leave checksum 0 now, filled later by pseudo header

    //Data part
    if(buf_len>sizeof(datagram)-sizeof(struct udphdr)-sizeof(struct pseudo_header))
    {
        snprintf(errmsg, sizeof(errmsg), "message too large to fit in a packet, buf_len=%d", buf_len);
        return -2;
    }
    memcpy(data, buf, buf_len);

    //Now the UDP checksum using the pseudo header
    psh->source_address = src_addr->sin_addr.s_addr;
    psh->dest_address = dst_addr->sin_addr.s_addr;
    psh->placeholder = 0;
    psh->protocol = IPPROTO_UDP;
    psh->udp_length = htons(sizeof(struct udphdr)+buf_len); // len field is calculated twice, one here, one in udp header

    udph->check = csum((unsigned short*)datagram , sizeof(struct pseudo_header)+sizeof(struct udphdr)+buf_len);

    //Send the packeta
    int ret = sendto(s, datagram+sizeof(struct pseudo_header), sizeof(struct udphdr)+buf_len,  0, (struct sockaddr *)dst_addr, sizeof(*dst_addr));
    if(ret<0)
    {
        snprintf(errmsg, sizeof(errmsg), "sendto failed: %s", strerror(errno));
        return -3;
    }

    if(ret<sizeof(struct udphdr))
    {
        snprintf(errmsg, sizeof(errmsg), "not enough bytes (%d) sent, please try again", ret);
        return -4;
    }

    // bytes of user payload sent
    return ret-sizeof(struct udphdr);
}

const char *raw_udp_errorstr()
{
    return errmsg;
}
