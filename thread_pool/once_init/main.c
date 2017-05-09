#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define handle_err_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while(0);

extern char *mystrerror(int err);

static void *thread_func(void *arg)
{
    char *str;
    printf("Other thread about to call mystrerror()\n");

    str = mystrerror(EPERM);
    printf("Other thread: str (%p) = %s\n", str, str);

    return NULL;
}

int main(int argc, char *argv[])
{
    char *str;
    str = mystrerror(EINVAL);
    printf("Main thread has called mystrerror()\n");

    int s;
    pthread_t t;
    s = pthread_create(&t, NULL, thread_func, NULL);
    if (s != 0) {
        handle_err_en(s, "pthread_create");
    }

    s = pthread_join(t, NULL);
    if (s != 0) {
        handle_err_en(s, "pthread_join");
    }

    printf("Main thread: str (%p) = %s\n", str, str);

    exit(EXIT_SUCCESS);
}
