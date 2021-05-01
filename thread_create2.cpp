#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

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
}

int main()
{
    pthread_t tid;
    
    for(int i = 0; i < 4; i++)
    {
        struct ThreadId* ti = new ThreadId();
        ti->thread_id_ = i;
        
        int ret = pthread_create(&tid, NULL, MyThreadStart, (void*)ti);
        if(ret < 0)
        {
            perror("pthread_create fail");
            return 0;
        }
    }

    //create success
    while(1)
    {
        printf("i am main thread\n");
        sleep(1);
    }

    return 0;
}
