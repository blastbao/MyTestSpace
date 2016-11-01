#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

int main()
{
	const char *fifo_name = "/tmp/my_fifo";
	int pipe_fd = -1;
	int data_fd = -1;
	int res = 0;
	int bytes_sent = 0;
	char buffer[PIPE_BUF + 1];

	//创建命名管道必须要求管道文件不存在。
	if(access(fifo_name, F_OK) == -1)
	{
		//若管道文件不存在，创建命名管道。
		//mkfifo函数如果调用成功的话，返回值为0；如果调用失败返回值为-1。
		res = mkfifo(fifo_name, 0777);
		if(res != 0)
		{
			fprintf(stderr, "Could not create fifo %s\n", fifo_name);
			exit(EXIT_FAILURE);
		}
	}

	printf("Process %d opening FIFO O_WRONLY\n", getpid());
	pipe_fd = open(fifo_name, O_WRONLY);  //以只写阻塞方式打开FIFO文件，
	data_fd = open("Data.txt", O_RDONLY); //以只读方式打开数据文件
	printf("Process %d result %d\n", getpid(), pipe_fd);


	//循环的从数据文件中读数据，而后写入到管道中，直到数据文件被全部读取完毕。
	if(pipe_fd != -1)
	{
		int bytes_read = 0;
		//从数据文件读取数据到内存缓冲区
		bytes_read = read(data_fd, buffer, PIPE_BUF);
		buffer[bytes_read] = '\0';
		while(bytes_read > 0)
		{
			//向FIFO文件写数据
			res = write(pipe_fd, buffer, bytes_read);
			if(res == -1)
			{
				fprintf(stderr, "Write error on pipe\n");
				exit(EXIT_FAILURE);
			}
			//累加写的字节数，并继续读取数据
			bytes_sent += res;
			bytes_read = read(data_fd, buffer, PIPE_BUF);
			buffer[bytes_read] = '\0';
		}
		close(pipe_fd);
		close(data_fd);
	}
	else
		exit(EXIT_FAILURE);

	printf("Process %d finished\n", getpid());
	exit(EXIT_SUCCESS);
}