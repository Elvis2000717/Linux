#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* MyThreadStart(void* arg)
{
    while(1)
    {
        printf("i am MyThreadStart\n");
        sleep(1);
    }
}

int main()
{
    pthread_t tid;

    int ret = pthread_create(&tid, NULL, MyThreadStart, NULL);
    if(ret < 0)
    {
        perror("pthread_create fail");
        return 0;
    }

    while(1)
    {
        printf("i am main\n");
        sleep(1);
    }

    return 0;
}
