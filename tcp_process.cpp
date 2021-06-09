#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

void sigcallback(int signo)
{
    wait(NULL);
}

int main()
{
    signal(SIGCHLD, sigcallback);
    
    int listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listen_sock < 0)
    {
        perror("socket");
        return 0;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(18989);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    int ret = bind(listen_sock, (struct sockaddr*)&addr, sizeof(addr));
    if(ret < 0)
    {
        perror("bind");
        return 0;
    }

    ret = listen(listen_sock, 1);
    if(ret < 0)
    {
        perror("listen");
        return 0;
    }

    while(1)
    {
        struct sockaddr_in peer_addr;
        socklen_t peer_addrlen = sizeof(peer_addr);
        int new_sock = accept(listen_sock, (struct sockaddr*)&peer_addr, &peer_addrlen);
        if(new_sock < 0)
        {
            perror("accept");
            return 0;
        }
        
        printf("accept %s:%d, create new_sock %d\n", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port), new_sock);

        pid_t pid = fork();
        if(pid < 0)
        {
            perror("fork");
            close(new_sock);
            continue;
        }
        else if(pid == 0)
        {
            //child
            
            close(listen_sock);
            while(1)
            {
                char buf[1024] = {0};
                ssize_t recv_size = recv(new_sock, buf, strlen(buf) - 1, 0);
                if(recv_size < 0)
                {
                    perror("recv");
                    continue;
                }
                else if(recv_size == 0)
                {
                    printf("peer shutdown");
                    close(new_sock);
                    return 0;
                }
                
                printf("client %d say： %s\n", new_sock, buf);

                memset(buf, '\0', sizeof(buf));
                sprintf(buf, "i am server, hi client %d\n", new_sock);
                ssize_t send_size = send(new_sock, buf, strlen(buf), 0);
                if(send_size < 0)
                {
                    perror("send");
                    continue;
                }
            }
        }
        else
        {
            //father
            close(new_sock);
            continue;
        }
    }
    close(listen_sock);

    return 0;
}
