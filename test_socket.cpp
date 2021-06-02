#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sockfd < 0)
    {
        perror("socket");
        return 0;
    }

    printf("sockfd: %d\n", sockfd);
    while(1)
    {
        sleep(1);
    }

    return 0;
}
