 ///
 /// @file	  server_select.c
 /// @author  mxk
 /// @date    2019-05-24 14:59:05
 ///
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXLINE 1024

int main(int argc, char** argv)
{
	int i, maxi, maxfd, listenfd, connfd, sockfd;
	int nready, client[FD_SETSIZE];
	ssize_t n;
	fd_set rset, allset;
	char buf[MAXLINE];
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0)
	{
		perror("socket")	;
		return -1;
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8888);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int nRet = bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if(nRet < 0)
	{
		perror("socket")	;
		return -1;
	}
	nRet = listen(listenfd, 10);
	if(nRet < 0)
	{
		perror("socket")	;
		return -1;
	}
	maxfd = listenfd;
	maxi = -1;
	for(i = 0; i < FD_SETSIZE; ++i)
	{
		client[i] = -1;
	}
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	for(;;)
	{
		rset = allset;	
		nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
		if(nready < 0)
		{
			if(errno == EINTR)	
			{
				continue;
			}
			perror("select");
			break;
		}
		if(FD_ISSET(listenfd, &rset))
		{
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
			for(i = 0; i < FD_SETSIZE; ++i)
			{
				if(client[i] < 0)	
				{
					client[i] = connfd;	
					break;
				}
			}
			if(i == FD_SETSIZE)
			{
				printf("too many clients")	;
				break;
			}
			printf("new client is connected.\n");
			FD_SET(connfd, &allset);
			if(connfd > maxfd)
			{
				maxfd = connfd;	
			}
//////////**********max index in client array***********//////////////////////////
			if(i > maxi)
			{
				maxi = i;	
			}
			/////*****no more readable descriptors****/////
			if(--nready <= 0)
			{
				continue;	
			}
		}
		for(i = 0; i <= maxi; ++i)
		{
			if((sockfd = client[i]) < 0)	
				continue;
			if(FD_ISSET(sockfd, &rset))
			{
				if((n = read(sockfd, buf, MAXLINE)) == 0)	
				{
					printf("client is close\n");
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				}
				else
				{
write:
					n = write(sockfd, buf, n);
					if(n < 0 && errno == EINTR)
					{
						goto write;	
					}

				}
				if(--nready <= 0)
				{
					break;	
				}
			}
		}
	}


}
