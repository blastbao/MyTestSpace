#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <string.h>
#include <unistd.h>
 
void sig_handler(int signum)
{
    printf("in signal handler...\n");
    sleep(1);
    printf("signal handler return\n");
}

//闹钟信号SIGALRM中断read系统调用。
//安装SIGALRM信号时如果不设置SA_RESTART属性，信号会中断read系统过调用。
//如果设置了SA_RESTART属性，read就能够自己恢复系统调用，不会产生EINTR错误。
int main(int argc, char **argv)
{
    char buf[100];
    int ret;
    struct sigaction action, old_action;
 
    action.sa_handler = sig_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    
    /* 版本1:不设置SA_RESTART属性
     * 版本2:设置SA_RESTART属性 */
    action.sa_flags |= SA_RESTART;
 
    sigaction(SIGALRM, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN) {
        sigaction(SIGALRM, &action, NULL);
    }

    //定时3秒钟触发sigalrm信号，这3秒内read操作在阻塞中，sigalrm会中断read操作。
    alarm(3);

    //未设置SA_RESTART标志：
    //read以阻塞方式读取标准输入中的数据，如果3秒之内没有从键盘输入数据，read操作会被sigalrm中断，在中断处理返回时，read函数会返回-1同时设置errno。
    //如果3秒之内有数据从键盘（标准输入0）输入，那么read会直接返回，程序会return 0 结束，这样就不收定时器信号的影响。
    //
    //已设置SA_RESTART标志：
    //read以阻塞方式读取标准输入中的数据，如果3秒之内没有从键盘输入数据，read操作会被sigalrm中断，在中断处理返回时，因为设置了SA_RESTART标志，会尝试
    //恢复被中断了的read操作，使得read操作继续之前阻塞式的等待键盘输入的状态，而不会返回错误码。
    //注：值得注意的是SA_RESTART标志不一定能保证所有的阻塞式调用能够恢复，比如msgrcv...
    bzero(buf, 100);
    ret = read(0, buf, 100);
    if (ret < 0) {
        perror("error occur when read");
    }else if (ret == 0){
        printf("connection closed \n");
    }else{
        printf("read %d bytes:\n", ret);
        printf("%s\n", buf);  
    }
    return 0;
}