#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define SER_IP "127.0.0.1"
#define SER_PORT 9877
#define MAX_LINE 128
void send_myself(int fd)
{
	char tmp[MAX_LINE] = {0};
	int size;
	fgets(tmp,MAX_LINE - 1,stdin);
	write(fd,tmp, strlen(tmp));
	bzero(tmp,MAX_LINE);
	//write(STDOUT_FILENO, tmp, MAX_LINE);
	//write(STDOUT_FILENO,"\n",strlen("\n"));
	if((size = read(fd,tmp,MAX_LINE -1)) == 0)
	{
		perror("read serv fail");
		return ;
	}
	//tmp[size] = '\n';
	//fputs(tmp,stdout);
	//write(STDOUT_FILENO, tmp, strlen(tmp));
	printf("size is %d\n",size);
	printf("%s",tmp);
	fflush(stdout);
	return ;
	
}
int main(int argc, char** argv)
{
	int send_sk;
	struct sockaddr_in s_addr;
	socklen_t len = sizeof(s_addr);
	send_sk = socket(AF_INET, SOCK_STREAM, 0);
	if(send_sk == -1)
	{
		perror("socket failed  ");
		return -1;
	}
	bzero(&s_addr, sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	//s_addr.sin_addr.s_addr = htonl(
	inet_pton(AF_INET,SER_IP,&s_addr.sin_addr);
	s_addr.sin_port = htons(SER_PORT);
	if(connect(send_sk,(struct sockaddr*)&s_addr,len) == -1)
	{
		perror("connect fail  ");
		return -1;
	}
	while(1)
	{
		send_myself(send_sk);	
	}
}

