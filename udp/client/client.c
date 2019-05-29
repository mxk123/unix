#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAXLINE 1024

void fun(FILE* fp, int fd, struct sockaddr* seraddr, int serLen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];
    while(fgets(sendline, MAXLINE, fp) != NULL)
    {
        n = sendto(fd, sendline, strlen(sendline), 0, seraddr, serLen) ;
        printf("sendto %d \n", n);
        n = recvfrom(fd, recvline, MAXLINE, 0, NULL, NULL);
        printf("recvfrom %d \n", n);
        recvline[n] = '\0';
        fputs(recvline, stdout);
    }
}

int main(int argc, char** argv)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in seraddr;
    bzero(&seraddr, sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(8888);
    seraddr.sin_addr.s_addr = inet_addr("192.168.188.130");

    fun(stdin,sockfd, (struct sockaddr*)&seraddr, sizeof(seraddr));

    close(sockfd);

    return 0;
}
