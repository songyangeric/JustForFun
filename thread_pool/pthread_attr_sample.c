#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while(0)

static void display_pthread_attr(pthread_attr_t *attr, char *prefix)
{
    int s, detach_state;
    size_t stack_size;
    void *stack_addr;
    struct sched_param sp;

    s = pthread_attr_getdetachstate(attr, &detach_state);
    if (s != 0) {
        handle_error_en(s, "pthread_attr_getdetachstate");
    }
    printf("%sDetach state = %s\n", prefix,
            (detach_state == PTHREAD_CREATE_DETACHED) ? "PTHREAD_CREATE_DETACHED" : (detach_state == PTHREAD_CREATE_JOINABLE) ? 
            "PTHREAD_CREATE_JOINABLE" : "???");

    s = pthread_attr_getstack(attr, &stack_addr, &stack_size);
    if (s != 0) {
        handle_error_en(s, "pthread_attr_getstack");
    }

    printf("%sStack address = %p\n", prefix, stack_addr);
    printf("%sStack size = 0x%x bytes\n", prefix, (unsigned)stack_size);
}

static void *thread_func(void *arg)
{
    int s;
    pthread_attr_t attr;

    s = pthread_getattr_np(pthread_self(), &attr);
    if (s != 0) {
        handle_error_en(s, "pthread_getattr_np");
    }

    printf("thread attributes:\n");
    display_pthread_attr(&attr, "\t");

    exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[])
{
    pthread_t thr;
    pthread_attr_t attr;
    
    int s;

    s = pthread_attr_init(&attr);
    if (s != 0) {
        handle_error_en(s, "pthread_attr_init");
    }

    s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (s != 0) {
        handle_error_en(s, "pthread_attr_setdetachstate");
    }

    if (argc > 1) {
        void *sp;
        int stack_size;

        stack_size = strtoul(argv[1], NULL, 0);
        s = posix_memalign(&sp, sysconf(_SC_PAGESIZE), stack_size);
        if (s != 0) {
            handle_error_en(s, "posix_memalign");
        }

        printf("posix_memalign() allocated at %p\n", sp);
    
        s = pthread_attr_setstack(&attr, sp, stack_size);
        if (s != 0) {
            handle_error_en(s, "pthread_attr_setstack");
        }
    }
    
    s = pthread_create(&thr, &attr, &thread_func, NULL);
    if (s != 0) {
        handle_error_en(s, "pthread_create");
    }

    s = pthread_attr_destroy(&attr);
    if (s != 0) {
        handle_error_en(s, "pthread_attr_destroy");
    }

    pause();
}
