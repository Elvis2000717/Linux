#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <iostream>
#include <queue>

using namespace std;

//设计线程安全队列的类型

typedef void (*Handler)(int);

class QueueType
{
    public:
        QueueType(int data, Handler handler)
        {
            data_ = data;
            handler_ = handler;
        }

        ~QueueType()
        {

        }

        //线程从线程池当中的线程安全队列获取一个元素之后
        //拿到该类型，通过该类型当中“deal”方法处理数据
        void Deal()
        {
            handler_(data_);
        }
    private:
        //待要处理的数据
        int data_;
        //处理数据的方法
        Handler handler_;
};

class MyThreadPool
{
    public:
        MyThreadPool()
        {

        }

        ~MyThreadPool()
        {

        }

        void StartThreadPool()
        {

        }

        void Push(QueueType* q)
        {

        }

        void Pop(QueueType** q)
        {

        }
    private:
        queue<QueueType*> que_;
        size_t capacity_;

        pthread_mutex_t lock_;
        pthread_cond_t cons_cond_;
        pthread_cond_t prod_cond_;

        int thread_count_;
};
