#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static int glob = 0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void thread_fun() {
    while (1) {
        pthread_mutex_lock(&mtx);
        glob++;
        printf("%lu incremented glob to %d\n", pthread_self(), glob);
        pthread_mutex_unlock(&mtx);
    }
}

int main(int argc, char** argv) {
    pthread_t* threads = calloc(1,sizeof(pthread_t));

    for (int i = 0; i < 10; i++) {
        pthread_create(threads, NULL, &thread_fun, NULL);
    }

    while(1);

    free(threads);
}
