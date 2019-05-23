#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>

#define MAXLEN 1024

void sig_chld(int signo)
{
	pid_t pid;
	int stat;
	
//	pid = wait(&stat);
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);
	return;
}

void func(int fd)
{
    ssize_t n;
    char buf[MAXLEN] = {0};
begin:
    while((n = read(fd, buf, MAXLEN)) > 0)
    {
        write(fd, buf, n);
    }
    
    if(n < 0 && errno == EINTR)
    {
        goto begin; 
    }
	else if(n < 0)
	{
		perror("read")	;
	}

}

int main()
{
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0)
    {
        perror("socket") ;
        return -1;
    }

    struct sockaddr_in ser, cli;
    bzero(&ser, sizeof(ser));

    ser.sin_family = AF_INET;
    ser.sin_port = htons(8888);
    ser.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listenfd, (struct sockaddr*)&ser, sizeof(ser)) < 0)
    {
        perror("bind") ;
        close(listenfd);
        return -1;
    }
    if(listen(listenfd, 10) < 0)
    {
        perror("listen") ;
        close(listenfd);
        return -1;
    }
	signal(SIGCHLD, sig_chld);
//	sleep(10);
    for(;;)
    {
        int len = sizeof(cli);
        int connfd = accept(listenfd, (struct sockaddr*)&cli, &len) ;
		if(connfd < 0 && errno == EINTR)
		{
			continue;
		}
		else if(connfd < 0)
		{
			perror("accept")	;
			break;
		}

        if(fork() == 0)
        {
			printf("child pid = %d\n", getpid());
            close(listenfd) ;
            func(connfd);
            close(connfd);
            exit(0);
        }
        else
        {
            close(connfd) ;
        }
    }
    close(listenfd);
    return 0;
}
