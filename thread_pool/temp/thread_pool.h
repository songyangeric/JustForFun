#ifndef _THREADPOOL_
#define _THREADPOOL_
#include <pthread.h>

typedef struct threadpool_work
{
    void (*process)(void*);
    void* arg;
    struct threadpool_work* next; //use the link for save thread.

}tpool_work_t;

typedef struct tpool
{
    int num_thread;//count of thread.
    int max_queue_size;  //max number of thread.

    pthread_t* tid; //thread id;
    tpool_work_t *queue; //the queue header.
    int front,rear; //the header and end.

    int closed; //close putting ,but can run the existed threads.
    int shutdown;   //shutdown all thread works.

    pthread_mutex_t queue_lock;      //lock the queue
    pthread_cond_t queue_has_task;   //task condition.
    pthread_cond_t queue_has_space;  //space condition
    pthread_cond_t queue_empty;      //empty condition

}*tpool_t;

void *thread_creat(void*); //void* pointer.

void tpool_init(tpool_t* tpool_p, int num_thread ,int max_queue_size);

int tpool_add_work(tpool_t tpool,void(*routine)(void*),void* arg); //第二个参数是任务函数的地址，*arg是任务函数的参数、

int tpool_destroy(tpool_t tpool, int finish); 
int get_size(tpool_t pool);
int full(tpool_t pool);
int empty(tpool_t pool);

#endif
