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
#define WAIT_COUNT 5
#define READ_SIZE 257

void send_myself(int fd);
void sig_child(int signo)
{
	int stat;
	pid_t pid = wait(&stat);
	printf("pid is %d\n",pid);
	return;
}
int main(int argc, char** argv)
{
	int listen_fd, real_fd;
	struct sockaddr_in listen_addr, client_addr;
	socklen_t len = sizeof(struct sockaddr_in);
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_fd == -1)
	{
		perror("socket failed   ");
		return -1;
	}
	signal(SIGCHLD, sig_child);
	bzero(&listen_addr,sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	listen_addr.sin_port = htons(SERV_PORT);
	bind(listen_fd,(struct sockaddr *)&listen_addr, len);
	listen(listen_fd, WAIT_COUNT);
    while(1)
	{
		real_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &len);
		if(real_fd == -1)
		{
			perror("accpet fail  ");
			return -1;
		}
		if(fork() == 0)
		{
			close(listen_fd);
			send_myself(real_fd);
			close(real_fd);
			exit(0);			
		}
		close(real_fd);
	}	
	return 0;
}
void send_myself(int fd)
{
	char tmp[READ_SIZE] = {0};
	while(1)
	{
		int size = read(fd ,tmp,READ_SIZE-1);
		write(STDOUT_FILENO ,tmp,size);
		if(size == 0)
		{
			perror("read fail   ");
			return ;
		}
		if(write(fd,tmp,size) == 0)
		{
			perror("write client failed ");
			return ;
		}
		bzero(tmp,READ_SIZE);
	}
}
