#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <queue>

using namespace std;

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
            //插入数据，入队
            que_.push(data);
            //解锁
            pthread_mutex_unlock(&lock_);
        }

        void Pop(int* data)
        {
            //加锁
            pthread_mutex_lock(&lock_);
            //将数据传递给出参
            *data = que_.front();
            //将数据出队
            que_.pop();
            //解锁
            pthread_mutex_unlock(&lock_);
        }
    private:
        //模板 实例化一个int类型的队列
        queue<int> que_;
        //设置队列大小
        int capacity_;

        //定义一个锁资源
        pthread_mutex_t lock_;
        //定义一个生产者的条件变量
        pthread_cond_t cons_cond_;
        //定义一个消费者的条件变量
        pthread_cond_t prod_cond_;
};
