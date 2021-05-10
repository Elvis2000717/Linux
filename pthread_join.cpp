#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define THREAD_NUM 4

struct ThreadId
{
    int thread_id_;
};

void* MyThreadStart(void* arg)
{
    struct ThreadId* ti = (struct ThreadId*)arg;
    while(1)
    {
        printf("i am MyThreadStart, i = %d\n", ti->thread_id_);
        sleep(1);
    }
    delete ti;

    return NULL;
}

int main()
{
    pthread_t tid[THREAD_NUM];
    
    for(int i = 0; i < THREAD_NUM; i++)
    {
        struct ThreadId* ti = new ThreadId();
        ti->thread_id_ = i;

        int ret = pthread_create(&tid[i], NULL, MyThreadStart, (void*)ti);
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

    while(1)
    {
        printf("i am main thread\n");
        sleep(1);
    }

    return 0;
}

