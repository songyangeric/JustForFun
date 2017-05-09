#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); } while(0)

static void *thread_routine(void *arg)
{
    int ii;

    printf("New thread started.\n");
    for (ii = 1; ; ii++) {
        printf("Loop %d\n", ii);
        sleep(1);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    int s;
    pthread_t thr;
    void *res;

    s = pthread_create(&thr, NULL, thread_routine, NULL);
    if (s != 0) {
        handle_error_en(s, "pthread_create");
    }

    sleep(3);

    s = pthread_cancel(thr);
    if (s != 0) {
        handle_error_en(s, "pthread_cancel");
    }

    s = pthread_join(thr, &res);
    if (s != 0) {
        handle_error_en(s, "pthread_join");
    }
    if (res == PTHREAD_CANCELED) {
        printf("Thread was canceled\n");
    } else {
        printf("Thread was not canceld (should not happen)\n");
    }

    return 0;
}
