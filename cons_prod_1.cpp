#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <iostream>
#include <queue>

using namespace std;

#define THREAD_NUM 1

class RingQueue
{
    public:
        RingQueue()
        {
            //设置容量
            capacity_ = 10;
            //互斥锁初始化
            pthread_mutex_init(&lock_, NULL);
            //生产者的条件变量初始化
            pthread_cond_init(&cons_cond_, NULL);
            //消费者的条件变量初始化
            pthread_cond_init(&prod_cond_, NULL);
        }

        ~RingQueue()
        {
            //销毁互斥锁资源
            pthread_mutex_destroy(&lock_);
            //销毁生产者条件变量
            pthread_cond_destroy(&cons_cond_);
            //销毁消费者条件变量
            pthread_cond_destroy(&prod_cond_);
        }

        void Push(int data)
        {
            //加锁
            pthread_mutex_lock(&lock_);
            //队列中元素个数大于等于容量
            //不能再继续插入，将生产线程放入PCB等待队列
            while(que_.size() >= capacity_)
            {
                pthread_cond_wait(&prod_cond_, &lock_);
            }
            //插入数据，入队
            que_.push(data);
            printf("i product %d, i am %p\n", data, pthread_self());
            //解锁
            pthread_mutex_unlock(&lock_);
            //唤醒PCB等待队列的消费者
            pthread_cond_signal(&cons_cond_);
        }

        void Pop(int* data)
        {
            //加锁
            pthread_mutex_lock(&lock_);
            //判断队列中的是否还有元素
            //如果没有消费者进入PCB等待队列
            while(que_.empty())
            {
                pthread_cond_wait(&cons_cond_, &lock_);
            }
            //将数据传递给出参
            *data = que_.front();
            printf("i consume %d, i am %p\n", data, pthread_self());
            //将数据出队
            que_.pop();
            //解锁
            pthread_mutex_unlock(&lock_);
            //唤醒PCB等待队列的生产者
            pthread_cond_signal(&prod_cond_);
        }
    private:
        //模板 实例化一个int类型的队列
        queue<int> que_;
        //设置队列大小
        size_t capacity_;

        //定义一个锁资源
        pthread_mutex_t lock_;
        //定义一个生产者的条件变量
        pthread_cond_t cons_cond_;
        //定义一个消费者的条件变量
        pthread_cond_t prod_cond_;

        std::vector<int> vec_;
};

void* ConsumeStart(void* arg)
{
    RingQueue* rq = (RingQueue*)arg;
    while(1)
    {
        int data;
        rq->Pop(&data);
    }
    return NULL;
}

int data = 1;
pthread_mutex_t g_data_lock;

void* ProductStart(void* arg)
{
    RingQueue* rq = (RingQueue*)arg;
    while(1)
    {
        //这块需要加锁的原因是data是临界资源
        pthread_mutex_lock(&g_data_lock);
        rq->Push(data++);
        pthread_mutex_unlock(&g_data_lock);
    }
    return NULL;
}

int main()
{
    pthread_mutex_init(&g_data_lock, NULL);
    RingQueue* rq = new RingQueue();
    if(rq == NULL)
    {
        return 0;
    }

    pthread_t cons[THREAD_NUM];
    pthread_t prod[THREAD_NUM];

    for(int i = 0; i < THREAD_NUM; i++)
    {
        int ret = pthread_create(&cons[i], NULL, ConsumeStart, (void*)rq);
        if(ret < 0)
        {
            perror("pthread_create fail");
            return 0;
        }

        ret = pthread_create(&prod[i], NULL, ProductStart, (void*)rq);
        if(ret < 0)
        {
            perror("pthread_create fail");
            return 0;
        }
    }

    for(int i = 0; i < THREAD_NUM; i++)
    {
        pthread_join(cons[i], NULL);
        pthread_join(prod[i], NULL);
    }

    delete rq;
    pthread_mutex_destroy(&g_data_lock);

    return 0;
}
