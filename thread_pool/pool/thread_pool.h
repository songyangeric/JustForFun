#ifndef _THREAD_POOL_
#define _THREAD_POOL_

#include <stdlib.h>
#include <pthread.h>


typedef struct thread_pool_task {
    void (*process)(void *);
    void *arg;
}thread_pool_task_t;

typedef struct thread_pool {
    int num_thread;
    pthread_t *threads;

    thread_pool_task_t *queue;
    int max_queue_size;
    int front, rear;
    
    int shutdown;

    pthread_mutex_t queue_lock;
    pthread_cond_t queue_not_full;
    pthread_cond_t queue_not_empty;
}thread_pool_t;

void thread_pool_init(thread_pool_t *thread_pool, int num_thread, int max_queue_size);

void *thread_routine(void *);

int thread_pool_add(thread_pool_t *thread_pool, void (*routine)(void *), void *arg);

int thread_pool_destroy(thread_pool_t *thread_pool);

int thread_pool_full(thread_pool_t *thread_pool);

int thread_pool_empty(thread_pool_t *thread_pool);


#endif
