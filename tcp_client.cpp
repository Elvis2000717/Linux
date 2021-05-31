#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockfd < 0)
    {
        perror("listen_sock");
        return 0;
    }

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(18989);
    dest_addr.sin_addr.s_addr = inet_addr("49.232.211.4");
    int ret = connect(sockfd, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    if(ret < 0)
    {
        perror("connect");
        return 0;
    }

    while(1)
    {
        sleep(1);
        char buf[1024] = "i am tcp clients";
        ssize_t send_size = send(sockfd, buf, strlen(buf), 0);
        if(send_size < 0)
        {
            perror("send");
            continue;
        }

        memset(buf, '\0', sizeof(buf));
        ssize_t recv_size = recv(sockfd, buf, sizeof(buf) - 1, 0);
        if(recv_size < 0)
        {
            perror("recv");
            continue;
        }
        else if(recv_size == 0)
        {
            printf("server close\n");
            close(sockfd);
            return 0;
        }
        printf("server say: %s\n", buf);

    }
}
