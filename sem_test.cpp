#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <vector>

using namespace std;

#define CAPACITY 10
#define THREAD_NUM 2

class RingQueue
{
    public:
        RingQueue():vec_(CAPACITY)
    {
        //设置队列容量
        capacity_ = CAPACITY;

        //信号量初始化 线程间 初始化资源的数量为0
        sem_init(&sem_lock_, 0, 1);
        sem_init(&sem_read_, 0, 0);
        sem_init(&sem_write_, 0, capacity_);

        //设置读写下标
        pos_read_ = 0;
        pos_write_ = 0;
    }
    ~RingQueue()
    {
        //销毁信号量资源
        sem_destroy(&sem_lock_);

        sem_destroy(&sem_read_);
        sem_destroy(&sem_write_);
    }
    
    void Push(int data)
    {
        //对资源计数器进行减1操作
        //判断资源计数器的值是否大于0
        //大于0 接口返回执行下面的代码
        //小于0 将线程放在PCB等待队列阻塞起来
        sem_wait(&sem_write_);

        sem_wait(&sem_lock_);
        //将数据的值写入
        vec_[pos_write_] = data;
        //写的下标加1
        pos_write_ = (pos_write_ + 1) % capacity_;
        printf("i write %d, i am %p\n", data, pthread_self());
        
        //会对资源计数器进行加1操作
        //判断资源计数器的值是否小于0
        //小于0 通知PCB等待队列
        //大于0 不通知PCB等待队列
        sem_post(&sem_lock_);

        sem_post(&sem_read_);
    }

    void Pop(int* data)
    {
        sem_wait(&sem_read_);
        sem_wait(&sem_lock_);
        *data = vec_[pos_read_];
        pos_read_ = (pos_read_ + 1) % capacity_;
        printf("i read %d, i am %p\n", *data, pthread_self());
        sem_post(&sem_lock_);
        sem_post(&sem_write_);
    }


    private:
        vector<int> vec_;
        int capacity_;

        //保证互斥的信号量
        sem_t sem_lock_;
        //保证同步的信号量
        sem_t sem_write_;
        sem_t sem_read_;

        //标识读写的位置
        int pos_read_;
        int pos_write_;
};

void* ReadStart(void* arg)
{
    RingQueue* rq = (RingQueue*)arg;
    while(1)
    {
        int data;
        rq->Pop(&data);
    }
    return NULL;
}

int g_data = 1;
sem_t g_sem_lock_;

void* WriteStart(void* arg)
{
    RingQueue* rq = (RingQueue*)arg;
    while(1)
    {
        sem_wait(&g_sem_lock_);
        rq->Push(g_data++);
        sem_post(&g_sem_lock_);
    }
}

int main()
{
    sem_init(&g_sem_lock_, 0, 1);
    RingQueue* rq = new RingQueue();
    if(rq == NULL)
    {
        return 0;
    }

    pthread_t cons[THREAD_NUM];
    pthread_t prod[THREAD_NUM];

    for(int i = 0; i < THREAD_NUM; i++)
    {
        int ret = pthread_create(&cons[i], NULL, WriteStart, (void*)rq);
        if(ret < 0)
        {
            perror("pthread_create fail");
            return 0;
        }

        ret = pthread_create(&prod[i], NULL, ReadStart, (void*)rq);
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
    sem_destroy(&g_sem_lock_);

    return 0;
}
