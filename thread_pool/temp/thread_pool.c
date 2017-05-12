#include <stdlib.h>
#include <stdio.h>
#include "thread_pool.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

void tpool_init(tpool_t* tpool_p, int num_thread ,int max_queue_size)
{
    int i;
    tpool_t pool;

    pool = (tpool_t)malloc(sizeof(struct tpool));
    if (pool ==NULL)
    {
        perror("malloc");
        exit(0);//return normal?
    }

    pool->num_thread = 0;
    pool->max_queue_size = max_queue_size+1; //为了留出一个位置，用于区别队列满和空的区别，详情看博客中循环队列那篇。
    pool->num_thread = num_thread;
    pool->tid = NULL;                        //thread id
    pool->front = pool->rear = 0;
    pool->closed = pool->shutdown = 0;
    if (pthread_mutex_init(&pool->queue_lock,NULL) == -1)
    {
        perror("pthread_mutex_init");
        free(pool);
        exit(0);
    }
    if (pthread_cond_init(&pool->queue_has_task,NULL) == -1)
    {
        perror("pthread_cond_init:queue_has_task");
        free(pool);
        exit(0);
    }
    if (pthread_cond_init(&pool->queue_has_space,NULL) == -1)
    {
        perror("pthread_cond_init:queue_has_space");
        free(pool);
        exit(0);
    }
    if (pthread_cond_init(&pool->queue_empty,NULL) == -1)
    {
        perror("pthread_cond_init:queue_empty");
        free(pool);
        exit(0);
    }

    if ((pool->queue = malloc(sizeof(struct threadpool_work)*pool->max_queue_size)) == NULL)
    {
        perror("malloc pool->queue:");
        free(pool);
        exit(0);
    }

    if ((pool->tid = malloc(sizeof(pthread_t)*num_thread))==NULL)
    {
        perror("malloc pool->tid");
        free(pool->queue);
        free(pool);
        exit(0);
    }

    for (i = 0; i < num_thread; i++)
    {

        if (pthread_create(&pool->tid[i],NULL,thread_creat,(void*)pool)!=0)// 创建线程，执行实体为thread_creat函数。
        {
            perror("pthread_create error");
            exit(0);
        }
    }

    *tpool_p = pool;

}
void *thread_creat(void* arg) // use for creating threads.
{
    tpool_t pool = (tpool_t)(arg);
    tpool_work_t* work;

    // printf("thread_creat start\n");
    for(;;) // loop forever
    {
        pthread_mutex_lock(&pool->queue_lock);// 在操作数据之前，需要先lock
        while(empty(pool)&&!pool->shutdown) // if the queue is empty and wait for add.
        {
            // printf("I am sleep..\n");
            pthread_cond_wait(&pool->queue_has_task,&pool->queue_lock); 
        }
        // printf("I am awake\n");
        if (pool->shutdown ==1)
        {
            // printf("exit\n");
            pthread_mutex_unlock(&pool->queue_lock);
            pthread_exit(NULL);
        }
        int is_full = full(pool);  
        work = pool->queue+pool->front; // that is a queue
        // printf("pool->front:%d\n",pool->front );
        pool->front = (pool->front+1)%pool->max_queue_size;
        // printf("pool->front(after)%d\n",pool->front );

        if (is_full)
        {
            pthread_cond_broadcast(&pool->queue_has_space);//  broadcast to all thread that has been full.
        }
        if (empty(pool))
        {
            pthread_cond_signal(&pool->queue_empty);
        }
        pthread_mutex_unlock(&pool->queue_lock);// 读写完成，解锁
            (*(work->process))(work->arg);
    }
}

int tpool_add_work(tpool_t tpool,void(*process)(void*),void* arg)// 添加任务函数
{
    tpool_work_t *temp;
    pthread_mutex_lock(&tpool->queue_lock);

    while(full(tpool) && (!tpool->shutdown) &&(!tpool->closed))
    {
        // printf("queue full\n");
        pthread_cond_wait(&tpool->queue_has_space,&tpool->queue_lock);// if full and do not shutdown and do not closed ,then we wait.
    }   
    if(tpool->shutdown ||tpool->closed)
    {
        // printf("shutdown\n");
        pthread_mutex_unlock(&tpool->queue_lock);
        return -1;
    }

    int is_empty = empty(tpool);
    // printf("is empty:%d\n", is_empty);
    int size =get_size(tpool);
    // printf("len:%d\n",size);
    temp = tpool->queue +tpool->rear;//  add a node of link.
    temp->process = process;
    temp->arg = arg;
    tpool->rear = (tpool->rear+1)%tpool->max_queue_size;

    if (is_empty)
    {       
        // printf("signal has task\n");
        pthread_cond_broadcast(&tpool->queue_has_task);
        // printf("pthread_cond_broadcast\n");
    }
    pthread_cond_broadcast(&tpool->queue_has_task);
    pthread_mutex_unlock(&tpool->queue_lock);

    return 0;
}




int tpool_destroy(tpool_t tpool, int finish)
{
    // printf("tpool_destroy\n");
    int i;
    pthread_mutex_lock(&tpool->queue_lock);
    tpool->closed = 1;//  wait for all works done

        if (finish ==1)
        {
            // printf("wait for all work done\n");
            while(!empty(tpool))
            {
                pthread_cond_wait(&tpool->queue_empty,&tpool->queue_lock);
            }
        }
    tpool->shutdown = 1;
    pthread_mutex_unlock(&tpool->queue_lock);
    pthread_cond_broadcast(&tpool->queue_has_task);

    // printf("wait for worker thread exit\n");


    for (i = 0; i < tpool->num_thread; i++)
    {
        pthread_join(tpool->tid[i],NULL); //  All awake and find out shotdown ,and exit.
    }
    // printf("free thread pool\n");

    free(tpool->tid);
    free(tpool->queue);
    free(tpool);
}

int empty(tpool_t pool)
{
    return pool->front ==pool->rear;
}

int full(tpool_t pool)
{
    return ((pool->rear+1)%pool->max_queue_size ==pool->front);// +1用于区别循环链表中的full and empty
}
int get_size(tpool_t pool)
{
    return (pool->rear+pool->max_queue_size-pool->front)%pool->max_queue_size;
}
