#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/select.h>

#define max(a,b) (a)>(b)?(a):(b)
#define MAXLINE 1024
//test
void func(FILE* fd, int sockfd)
{
	int maxfdp1;
	fd_set rset;
	char recv[MAXLINE], send[MAXLINE] ;
	int n;
	int stdineof = 0;

	FD_ZERO(&rset);
	for(;;)
	{
		if(stdineof == 0)
			FD_SET(fileno(fd), &rset);
		FD_SET(sockfd,&rset);
		maxfdp1 = max(fileno(fd), sockfd) + 1;
		select(maxfdp1, &rset, NULL, NULL, NULL);

		if(FD_ISSET(sockfd, &rset))
		{
			n = read(sockfd, recv, MAXLINE)	;
			if(n < 0)
			{
				if(errno == EINTR)	
				{
					continue;	
				}
				else
				{
					perror("read")	;
					break;
				}
			}
			if(n == 0)
			{
				if(stdineof == -1)
				{
					printf("server close write.\n")	;
					break;
				}
				printf("server terminated prematurely\n");	
				break;
			}
			fputs(recv, stdout);
		}
		if(FD_ISSET(fileno(fd), &rset))
		{
			if((n = read(fileno(fd), send, MAXLINE)) == 0)
			{
				stdineof = -1;
				shutdown(sockfd, SHUT_WR);
				FD_CLR(fileno(fd), &rset);
				printf("read return 0\n");
				continue;
			}
			send[n] = '\0';
			write(sockfd, send, MAXLINE);
		}
	}
}

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        perror("argc");
        return -1;
    }
	
	int i, fd[5];
	for( i = 0; i < 1; ++i)
	{
		fd[i] = socket(AF_INET, SOCK_STREAM, 0);
		if(fd[i] < 0)
		{
			perror("socket") ;
			return -1;
		}

		struct sockaddr_in ser;
		bzero(&ser, sizeof(ser));
		ser.sin_family = AF_INET;
		ser.sin_port = htons(8888);
		ser.sin_addr.s_addr = inet_addr(argv[1]);
		int nRet = connect(fd[i], (struct sockaddr*)&ser, sizeof(ser));
		if(nRet < 0)
		{
			perror("connect") ;
			close(fd[i]);
			return -1;
		}
#if 0
		struct linger so_linger;
		so_linger.l_onoff = 1;
		so_linger.l_linger = 0;
		setsockopt(fd[i], SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger));
#endif	
	}
    func(stdin, fd[0]);
    return 0;
}
