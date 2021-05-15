#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_NUM 4

int g_bowl = 1;
pthread_mutex_t g_lock;
//eat
pthread_cond_t g_cond;
//make
pthread_cond_t g_make_cond;

//eat
void* MyThreadEat(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&g_lock);
        //判断是否能够吃
        while(g_bowl == 0)
        {
            //等待
            pthread_cond_wait(&g_cond, &g_lock);
        }
        printf("i eat %d, i am %p\n", g_bowl, pthread_self());
        g_bowl--;
        pthread_mutex_unlock(&g_lock);
        //通知做面
        pthread_cond_signal(&g_make_cond);
    }
    return NULL;
}

void* MyThreadMake(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&g_lock);
        //判断是否能够继续做面
        while(g_bowl == 1)
        {
            //等待
            pthread_cond_wait(&g_make_cond, &g_lock);
        }
        printf("i make %d, i am %p\n", g_bowl, pthread_self());
        g_bowl++;
        pthread_mutex_unlock(&g_lock);
        //通知吃面
        pthread_cond_signal(&g_cond);
    }
    return NULL;
}

int main()
{
   pthread_mutex_init(&g_lock, NULL);
   pthread_cond_init(&g_cond, NULL);
   pthread_cond_init(&g_make_cond, NULL);
   pthread_t tid_A[THREAD_NUM], tid_B[THREAD_NUM];
   for(int i = 0; i < THREAD_NUM; i++)
   {
       int ret = pthread_create(&tid_A[i], NULL, MyThreadEat, NULL);
       if(ret < 0)
       {
           perror("pthread_create");
           exit(0);
       }
       ret = pthread_create(&tid_B[i], NULL, MyThreadMake, NULL);
       if(ret < 0)
       {
           perror("pthread_create");
           exit(0);
       }
   }
   for(int i = 0; i < THREAD_NUM; i++)
   {
       pthread_join(tid_A[i], NULL);
       pthread_join(tid_B[i], NULL);
   }

   pthread_mutex_destroy(&g_lock);
   pthread_cond_destroy(&g_cond);
   pthread_cond_destroy(&g_make_cond);
    return 0;
}
