#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *thread_func1(void *arg)
{
    fprintf(stdout, "thread1: hello world.\n");
    sleep(1);

    return NULL;
}

void *thread_func2(void *arg)
{
    fprintf(stdout, "thread2: I'm running...\n");

    pthread_t main_thread = *(pthread_t *)arg;

    pthread_detach(pthread_self());

    if (!pthread_equal(main_thread, pthread_self())) {
        fprintf(stdout, "thread2: main thread id does not equal thread2.\n");
    }

    pthread_join(main_thread, NULL);
    fprintf(stdout, "thread2: main thread exits.\n");

    fprintf(stdout, "thread2: exit.\n");
    fprintf(stdout, "thread2: process exits.\n");

    pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
    pthread_t t1;
    pthread_t t2;

    if (pthread_create(&t1, NULL, thread_func1, NULL) !=0 ) {
        fprintf(stderr, "creating thread 1 failed.\n");
        exit(-1);
    }

    pthread_join(t1, NULL);
    fprintf(stdout, "thread 1 terminated.\n");

    pthread_t tid;
    if (pthread_create(&t2, NULL, thread_func2, (void *)&tid) != 0) {
        fprintf(stderr, "creating thread 2 failed.\n");
        exit(-1);
    }
    
    fprintf(stdout, "main thread: sleeping 3 secs...\n");
    sleep(3);

    // ensure that even the main thread exits, the process won't terminate
    fprintf(stdout, "main thread exit.\n");
    pthread_exit(NULL);

    fprintf(stdout, "main thread: never reach here\n");

    return 0;
}

