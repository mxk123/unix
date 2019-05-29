#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 1024

void func(int fd, struct sockaddr* cliaddr, int clilen) 
{
    char buf[MAXLINE];
    for(;;)
    {
        int n = recvfrom(fd,buf, MAXLINE, 0, cliaddr, &clilen ) ;
        if(n < 0)
        {
            perror("recvfrom");
            return;
        }
        else if(n == 0)
        {
            printf("recvfrom return 0\n");
            sleep(2);
        }
        printf("addr : %s, port : %d\n", inet_ntoa(((struct sockaddr_in*)cliaddr)->sin_addr), 
                ntohs(((struct sockaddr_in*)cliaddr)->sin_port));
        sendto(fd, buf, n, 0, cliaddr, clilen);
    }
}

int main(int argc, char** argv)
{
    int sockfd;
    struct sockaddr_in seraddr, cliaddr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&seraddr, sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(8888);
    seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(sockfd, (struct sockaddr*)&seraddr, sizeof(seraddr));

    func(sockfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr));

    close(sockfd);

    return 0;
}
