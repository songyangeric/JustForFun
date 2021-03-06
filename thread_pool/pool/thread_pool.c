#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include "thread_pool.h"

void *thread_routine(void *arg)
{
    thread_pool_t *pool = (thread_pool_t *)arg;
    thread_pool_task_t *task;

    for (;;) {
        pthread_mutex_lock(&pool->queue_lock);
        while (thread_pool_empty(pool) && !pool->shutdown) {
            pthread_cond_wait(&pool->queue_not_empty, &pool->queue_lock);
        }

        if (pool->shutdown) {
            pthread_mutex_unlock(&pool->queue_lock);
            pthread_exit(NULL);
        }

        task = &pool->queue[pool->front];
        pool->front = (pool->front + 1) % pool->max_queue_size;

        // pthread_cond_broadcast(&pool->queue_not_full);

        pthread_mutex_unlock(&pool->queue_lock);

        (*(task->process))(task->arg);
    }
}

void thread_pool_init(thread_pool_t *thread_pool, int num_thread, int max_queue_size)
{
    thread_pool_t *pool = thread_pool;
    pool = (thread_pool_t *)malloc(sizeof(thread_pool_t));
    if (pool == NULL) {
        perror("malloc");
        exit(-1);
    }
    memset(pool, 0, sizeof(thread_pool_t));

    pool->num_thread = num_thread;
    pool->max_queue_size = max_queue_size;
    pool->threads = NULL;
    pool->front = pool->rear = 0;
    pool->shutdown = 0;

    if (pthread_mutex_init(&pool->queue_lock, NULL) != 0) {
        perror("pthread_mutex_init");
        free(pool);
        exit(-1);
    }

    if (pthread_cond_init(&pool->queue_not_empty, NULL) != 0 
            || pthread_cond_init(&pool->queue_not_full, NULL) != 0) {
        perror("pthread_cond_init");
        free(pool);
        exit(-1);
    }

    if ((pool->queue = malloc(sizeof(thread_pool_task_t) * max_queue_size)) == NULL) {
        perror("malloc queue");
        free(pool);
        exit(-1);
    }
    memset(pool->queue, 0, sizeof(thread_pool_task_t) * max_queue_size);

    if ((pool->threads = malloc(sizeof(pthread_t) * num_thread)) == NULL) {
        perror("malloc threads");
        free(pool->queue);
        free(pool);
        exit(-1);
    }
    memset(pool->threads, 0, sizeof(pthread_t) * num_thread);

    int i;
    for (i = 0; i < num_thread; i++) {
        if (pthread_create(&pool->threads[i], NULL, thread_routine, (void *)pool) != 0) {
            perror("pthread_create");
            free(pool->threads);
            free(pool->queue);
            free(pool);
            exit(-1);
        }
    }
}

int thread_pool_add(thread_pool_t *pool, void (*process)(void *), void *arg)
{
    thread_pool_task_t *task;

    pthread_mutex_lock(&pool->queue_lock);

    while (thread_pool_full(pool) && (!pool->shutdown)) {
        pthread_cond_wait(&pool->queue_not_full, &pool->queue_lock);
    }

    if (pool->shutdown) {
        pthread_mutex_unlock(&pool->queue_lock);
        return -1;
    }

    task = &pool->queue[pool->rear];
    pool->rear = (pool->rear + 1) % pool->max_queue_size;
    task->process = process;
    task->arg = arg;

    pthread_cond_broadcast(&pool->queue_not_empty);

    pthread_mutex_unlock(&pool->queue_lock);

    return 0;
}

int thread_pool_empty(thread_pool_t *pool)
{
    return (pool->front == pool->rear);
}

int thread_pool_full(thread_pool_t *pool)
{
    return ((pool->rear + 1) % pool->max_queue_size == pool->front);
}

int thread_pool_destroy(thread_pool_t *pool)
{
    pthread_mutex_lock(&pool->queue_lock);

    pool->shutdown = 1;
    pthread_mutex_unlock(&pool->queue_lock);
    
    int i;
    for (i = 0; i < pool->num_thread; i++) {
        if (pthread_join(pool->threads[i], NULL) != 0) {
            perror("pthread_join");
            exit(-1);
        }
    }

    free(pool->threads);
    free(pool->queue);
    free(pool);

    return 0;
}


