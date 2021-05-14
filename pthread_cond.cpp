#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int g_bowl = 1;

pthread_mutex_t g_lock;

//eat
void* MyThreadA(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&g_lock);
        printf("i eat %d, i am %p\n", g_bowl, pthread_self());
        g_bowl--;
        pthread_mutex_unlock(&g_lock);
    }
    return NULL;
}

//make
void* MyThreadB(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&g_lock);
        printf("i make %d, i am %p\n", g_bowl, pthread_self());
        g_bowl++;
        pthread_mutex_unlock(&g_lock);
    }
    return NULL;
}

int main()
{
    pthread_mutex_init(&g_lock, NULL);
    pthread_t tid_A, tid_B;
    pthread_create(&tid_A, NULL, MyThreadA, NULL);
    pthread_create(&tid_B, NULL, MyThreadB, NULL);

    pthread_join(tid_A, NULL);
    pthread_join(tid_B, NULL);

    pthread_mutex_destroy(&g_lock);

    return 0;
}
