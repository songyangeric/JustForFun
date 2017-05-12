#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "thread_pool.h"

char *str[] ={"string 0","string 1","string 2","string 3","string 4","string 5","string 6",};

void job(void *str)
{
    long i,x =0;
    for (i = 0; i < 10000000; ++i)//loop for watching the print.
    {
        x=x+i;
    }
    printf("%s\n",(char*)str);
}
int main(int argc, char const *argv[])
{
    int i;
    tpool_t test_pool;

    tpool_init(&test_pool,8,18);//工作线程为8，最大为18个

    for (i = 0; i < 6; ++i)
    {
        //printf("loop\n");
        tpool_add_work(test_pool, job, str[i]);
    }
    tpool_destroy(test_pool,1);

    return 0;
}
