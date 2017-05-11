#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "thread_pool.h"

char *str[] = { "string 0", "string 1", "string 2", "string 3", "string 4", "string 5", "string 6"};

void task_routine(void *arg)
{
    int i;
    int x = 0;
    for (i = 0; i < 100000; i++) {
        x = x + i;
    }

    printf("%s\n", (char *)arg);
}

int main(int argc, char *argv[])
{
    thread_pool_t pool;
    thread_pool_init(&pool, 8, 18);

    int i;
    for (i = 0; i < 6; i++) {
        thread_pool_add(&pool, task_routine, str[i]);
    }

    thread_pool_destroy(&pool);

    return 0;
}
