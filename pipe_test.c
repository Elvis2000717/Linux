#include <stdio.h>
#include <unistd.h>

int main()
{
    int fd[2];
    int ret = pipe(fd);
    if(ret < 0)
    {
        perror("pipe");
        return 0;
    }

    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork");
        return 0;
    }
    else if(pid == 0)
    {
        //child
        /*
         * fd[0],fd[1]
         * 如果在子进程当中关闭fd[0],会不会影响父进程当中的fd[0]?
         * 结论：不会，进程独立性
         * */
        sleep(5);
        printf("i am child: fd[0]: %d, f[1]: %d\n",fd[0],fd[1]);
        //close(fd[0]);
        //close(fd[1]);
        //假设子进程进行读
        
        while(1)
        {
            //char buff[1024] = {0};
            //char buf[2] = {0};
            //read(fd[0], buf, sizeof(buf) - 1);
            //printf("i am child: buf is %s\n", buf);
            sleep(1);
        }
    }
    else
    {
        //father
        //fd[0], fd[1]
        printf("i am father: fd[0]:%d, fd[1]:%d\n", fd[0], fd[1]);

        int count = 0;
        while(1)
        {
            write(fd[1], "a", 1);
            printf("count:%d\n", ++count);
        }
        //write(fd[1], "bbb", 3);
    }

    while(1)
    {
        sleep(1);
    }

    return 0;
}
