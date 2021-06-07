#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    // int socket(int domain, int type, int protocol);
    // damain 当前的地址域使用什么协议
    //        AF_INET:ipv4
    //        AF_INET6:ipv6
    //        AF_UNIX:本地域套接字
    // type 创建套接字类型
    //        SOCK_DGRAM 用户数据报套接字 UDP协议
    //        SOCK_STREAM 流式套接字 TCP协议
    // protocol 使用什么类型的套接字协议
    //        0：表示套接字类型默认的协议
    //        IPPROTO_UDP:17
    //        IPPROTO_TCP:6
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sockfd < 0)
    {
        perror("socket failed");
        return 0;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    //主机字节序转换成为网络字节序
    addr.sin_port = htons(18989);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int bind_ret = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if(bind_ret < 0)
    {
        perror("bind failed");
        return 0;
    }

    while(1)
    {
        //接收
        
        char buf[1024] = {0};
        
        sockaddr_in addr_recvfrom;
        socklen_t addr_recvfromlen = sizeof(addr_recvfrom);
        //ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
        //                        struct sockaddr *src_addr, socklen_t *addrlen);
        ssize_t recvfrom_ret = recvfrom(sockfd, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&addr_recvfrom, &addr_recvfromlen);
        if(recvfrom_ret < 0)
        {
            continue;
        }
        printf("i am server, i recv %s from %s:%d\n", buf, inet_ntoa(addr_recvfrom.sin_addr), ntohs(addr_recvfrom.sin_port));

        //发送
        memset(buf, '\0', sizeof(buf));
        sprintf(buf, "client: %s : %d, i am server", inet_ntoa(addr_recvfrom.sin_addr), ntohs(addr_recvfrom.sin_port));

        sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&addr_recvfrom, sizeof(addr_recvfrom));
    }
    return 0;
}
