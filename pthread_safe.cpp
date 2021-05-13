#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define THREAD_NUM 2

int g_tickets = 100000;
pthread_mutex_t my_lock;

void* MyThreadStart(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&my_lock);
        if(g_tickets > 0)
        {
            printf("i have %d, i am %p\n", g_tickets, pthread_self());
            g_tickets--;
        }
        else
        {
            pthread_mutex_unlock(&my_lock);
            pthread_exit(NULL);
        }
        pthread_mutex_unlock(&my_lock);
    }
    return NULL;
}

int main()
{
    pthread_mutex_init(&my_lock, NULL);

    pthread_t tid[THREAD_NUM];
    for(int i = 0; i < THREAD_NUM; i++)
    {
        int ret = pthread_create(&tid[i], NULL, MyThreadStart, NULL);
        if(ret < 0)
        {
            perror("pthread_create");
            return 0;
        }
    }

    for(int i = 0; i < THREAD_NUM; i++)
    {
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&my_lock);
    printf("phread_join end...\n");

    return 0;
}
