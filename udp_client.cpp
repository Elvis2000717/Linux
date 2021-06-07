#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    //int socket(int domain, int type, int protocol);
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sockfd < 0)
    {
        perror("sockfd failed");
        return 0;
    }

    while(1)
    {
        sleep(1);
        char buf[1024] = "i am client";

        //通用结构体
        struct sockaddr_in  dest_addr;
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(18989);
        dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        //ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
        //                      const struct sockaddr *dest_addr, socklen_t addrlen);
        int sendto_ret = sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));

        if(sendto_ret < 0)
        {
            perror("sendto failed");
            return 0;
        }

        memset(buf, '\0', sizeof(buf));
        //ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
        //                        struct sockaddr *src_addr, socklen_t *addrlen);
        ssize_t recv_size = recvfrom(sockfd, buf, sizeof(buf) - 1, 0, NULL, NULL);
        if(recv_size < 0)
        {
            perror("recvfrom failed");
            continue;
        }
        printf("server say: %s\n", buf);
    }
    return 0;
}
