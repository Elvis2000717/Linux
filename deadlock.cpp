#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t lock_1;
pthread_mutex_t lock_2;

void* MyThread_A(void* arg)
{
    pthread_mutex_lock(&lock_1);
    sleep(2);
    pthread_mutex_lock(&lock_2);

    return NULL;
}

void* MyThread_B(void* arg)
{
    pthread_mutex_lock(&lock_2);
    sleep(2);
    pthread_mutex_lock(&lock_1);

    return NULL;
}

int main()
{
    pthread_mutex_init(&lock_1, NULL);
    pthread_mutex_init(&lock_2, NULL);

    pthread_t tid_A, tid_B;
    pthread_create(&tid_A, NULL, MyThread_A, NULL);
    pthread_create(&tid_B, NULL, MyThread_B, NULL);

    pthread_join(tid_A, NULL);
    pthread_join(tid_A, NULL);

    pthread_mutex_destroy(&lock_1);
    pthread_mutex_destroy(&lock_2);

    return 0;
}
