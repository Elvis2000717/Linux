#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <iostream>
#include <queue>

using namespace std;

//队列元素个数
#define QUEUE_SIZE 10
//线程个数
#define THREAD_NUM 4

//设计线程安全的类型
typedef void (*Handler)(int);

class QueueType
{
    public:
        //构造函数
        QueueType(int data, Handler handler)
        {
            data_ = data;
            handler_ = handler;
        }

        //析构函数
        ~QueueType()
        {

        }

        //线程从线程池当中当中线程安全队列获取一个元素之后
        //拿到了该类型，通过该类型当中的“deal”方法处理数据
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
        //构造函数
        MyThreadPool()
        {
            //默认队列容量
            capacity_ = QUEUE_SIZE;
            //动态初始化互斥锁
            pthread_mutex_init(&lock_, NULL);

            //动态初始化条件变量
            pthread_cond_init(&cons_cond_, NULL);
            pthread_cond_init(&prod_cond_, NULL);
            thread_count_ = THREAD_NUM;

            is_quit_ = false;
        }

        //析构函数
        ~MyThreadPool()
        {
            //销毁锁资源
            pthread_mutex_destroy(&lock_);
            //销毁消费者条件变量
            pthread_cond_destroy(&cons_cond_);
            //销毁生产者条件变量
            pthread_cond_destroy(&prod_cond_);
        }

        //线程池启动程序
        int InitAndStart(int thread_num = THREAD_NUM)
        {
            //如果没有传递进来线程数量，我们就用默认的线程数量
            //如果传递进来线程数量，我们就拿来和默认的进行比较，不相等就用
            //传递进来的线程数量
            if(thread_num != THREAD_NUM)
            {
                thread_count_ = thread_num;
            }

            //创建失败的线程数量
            int fail_count = 0;
            for(int i = 0; i < thread_count_; i++)
            {
                pthread_t tid;
                int ret = pthread_create(&tid, NULL, ThreadPoolStart, this);
                if(ret < 0)
                {
                    fail_count++;
                    printf("create thread failed, %d\n", i);
                    continue;
                }
            }
            //这里计算出来的线程数量是实际创建成功的线程数量
            thread_count_ -= fail_count;
            //如果线程数量为0，那么说明所有的创建都失败了
            if(thread_count_ == 0)
            {
                return -1;
            }
            return thread_count_;
        }

        static void* ThreadPoolStart(void* arg)
        {
            //分离线程
            //进程属性从joinable属性编程detach属性，当线程退出的时候，
            //不需要其他线程回收资源，操作系统会自己回收资源
            pthread_detach(pthread_self());
            MyThreadPool* mtp = (MyThreadPool*)arg;
            while(1)
            {
                pthread_mutex_lock(&mtp->lock_);
                //1.从队列当中获取
                while(mtp->que_.empty())
                {
                    if(mtp->is_quit_)
                    {
                        //因为判断线程需要突出，但此时当前线程拥有锁资源
                        //因此退出之前，需要解锁
                        pthread_mutex_unlock(&mtp->lock_);
                        pthread_exit(NULL);
                    }
                    //如果队列中为空，就需要等待生产
                    pthread_cond_wait(&mtp->cons_cond_, &mtp->lock_);
                }
                QueueType* q;
                mtp->Pop(&q);

                //先解锁可以提高程序的运行效率
                //因为数据已经从队列中获取走了，其他线程不能再拿到，因此
                //解锁后再处理数据也不会有二义性
                pthread_mutex_unlock(&mtp->lock_);
                //唤醒生产队列中的等待队列
                pthread_cond_signal(&mtp->prod_cond_);
                //2.处理数据
                q->Deal();

                //QueueType* qt = new QueueType(i, DealNum);
                //delete q: 释放的是上面的动态开辟的内存
                delete q;
            }
            return NULL;
        }

        int Push(QueueType* q)
        {
            pthread_mutex_lock(&lock_);
            //如果队列元素等于或者大于容量就不能再生产，进入等待队列
            while(que_.size() >= capacity_)
            {
                if(is_quit_)
                {
                    pthread_mutex_unlock(&lock_);

                    return -1;
                }
                pthread_cond_wait(&prod_cond_, &lock_);
            }

            if(is_quit_)
            {
                pthread_mutex_unlock(&lock_);
                return -1;
            }
            que_.push(q);
            pthread_mutex_unlock(&lock_);
            pthread_cond_signal(&cons_cond_);
            return 0;
        }

        void Pop(QueueType** q)
        {
            *q = que_.front();
            que_.pop();
        }

        void ThreadExit()
        {
            pthread_mutex_lock(&lock_);
            if(is_quit_ == false)
            {
                is_quit_ = true;
            }
            pthread_mutex_unlock(&lock_);

            //broadcast
            pthread_cond_broadcast(&cons_cond_);
        }
    private:
        //实例化队列
        queue<QueueType*> que_;
        //容量
        size_t capacity_;

        //定义一个锁资源
        pthread_mutex_t lock_;
        //定义消费者条件变量
        pthread_cond_t cons_cond_;
        //定义生产者条件变量
        pthread_cond_t prod_cond_;

        //线程数量
        int thread_count_;
        //是否退出
        bool is_quit_;
};

void DealNum(int data)
{
    printf("i am DealNum func, i deal %d\n", data);
}

int main(int argc, char* argv[])
{
    int thread_num = 0;
    for(int i = 0; i <argc; i++)
    {
        if(strcmp(argv[i], "-thread_num") == 0 && i + 1 < argc)
        {
            thread_num = atoi(argv[i + 1]);
        }
    }

    MyThreadPool* mtp = new MyThreadPool();
    if(mtp == NULL)
    {
        printf("create threadpool failed\n");
        return 0;
    }

    int ret = mtp->InitAndStart(thread_num);
    if(ret < 0)
    {
        printf("thread all create failed\n");
        return 0;
    }

    printf("create success, thread_count: %d\n", ret);

    for(int i = 0; i < 10000; i++)
    {
        //向线程池当中的线程安全队列当中放数据
        QueueType* qt = new QueueType(i, DealNum);
        if(qt == NULL)
        {
            continue;
        }
        mtp->Push(qt);
    }
    mtp->ThreadExit();

    delete mtp;

    while(1)
    {
        sleep(1);
    }
    return 0;
}
