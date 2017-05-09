#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while(0)

static int glob = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void *thread_func(void *arg)
{
    int ii;
    for (ii = 0; ii < 100000; ii++) {
        pthread_mutex_lock(&mtx);
        glob++;
        pthread_mutex_unlock(&mtx);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t t1, t2;
    int s;

    s = pthread_create(&t1, NULL, thread_func, NULL);
    if (s != 0) {
        handle_error_en(s, "pthread_create");
    }
    s = pthread_create(&t2, NULL, thread_func, NULL);
    if (s != 0) {
        handle_error_en(s, "pthread_create");
    }

    s = pthread_join(t1, NULL);
    if (s != 0) {
        handle_error_en(s, "pthread_join");
    }
    s = pthread_join(t2, NULL);
    if (s != 0) {
        handle_error_en(s, "pthread_join");
    }

    printf("glob = %d\n", glob);

    exit(EXIT_SUCCESS);
}


