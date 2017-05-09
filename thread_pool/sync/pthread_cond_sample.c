#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static pthread_t threads[2];
static write_char[2] = {'A', 'B'};

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct file_res {
    pthread_t *writer;
    int fd;
} file_res = {
    .writer = &threads[0]
};

void *write_routine(void *arg)
{
    int index = (intptr_t)arg;
    int next_index = 0;
    printf("thread %d is running, and will write '%c' to file.\n", index, write_char[index]);

    while (1) {
        if (pthread_mutex_lock(&mtx)) {
            exit(-1);
        }
        for (;;) {
            if (&threads[index] == file_res.writer) {
                write(file_res.fd, &write_char[index], sizeof(write_char[index]));

                next_index = (index + 1) % 2;
                file_res.writer = &threads[next_index];

                break;
            }

            pthread_cond_wait(&cond, &mtx);
        }

        if (pthread_mutex_unlock(&mtx)) {
            exit(-1);
        }

        pthread_cond_signal(&cond);
    }
}

int main(int argc, char *argv[])
{
    char file_name[] = "file";
    if ((file_res.fd = open(file_name, O_RDWR|O_CREAT|O_TRUNC, 0666)) < 0) {
        printf("opening %s failed\n", file_name);
        exit(-1); 
    }

    int i;
    for (i = 0; i < (sizeof(threads) / sizeof(*threads)); i++) {
        if (pthread_create(&threads[i], NULL, write_routine, (void *)(intptr_t)i)) {
            printf("creating write thread failed.\n");
            exit(-1);
        }
    }
    
    pthread_exit(NULL);
}
