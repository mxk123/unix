#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 1024

int main(int argc, char** argv)
{
    int sockfd;
    struct sockaddr_in seraddr, cliaddr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&seraddr, sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(46890);
    seraddr.sin_addr.s_addr = inet_addr("192.168.188.130");

    for(;;)
    {
        sendto(sockfd, "hello", 5, 0, (struct sockaddr*)&seraddr, sizeof(seraddr)) ;
        sleep(2);
    }
    close(sockfd);

    return 0;
}
