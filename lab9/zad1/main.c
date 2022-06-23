#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define ELVES 10
#define ELVES_QUEUE_LEN 3

int elves_in_queue = 0;
int elves_queue[ELVES_QUEUE_LEN] = {-1, -1, -1};

pthread_mutex_t elve_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t elves_cond = PTHREAD_COND_INITIALIZER;


void santa_func(void* arg) {

    // no bo w tej wersji nie dostarcza prezentow wiec dziala ciagle
    while (1) {

        pthread_mutex_lock(&santa_mutex);
        while (elves_in_queue < ELVES_QUEUE_LEN) {
            pthread_cond_wait(&santa_cond, &santa_mutex);
        }
        pthread_mutex_unlock(&santa_mutex);

        printf("SANTA CLAUS IS WAKING UP!\n");

        pthread_mutex_lock(&elve_mutex);
        if (elves_in_queue == ELVES_QUEUE_LEN) {
            printf("SANTA IS SOLVING ELVES (%d %d %d) problems\n", elves_queue[0], elves_queue[1], elves_queue[2]);
            elves_in_queue = 0;
            sleep(2);
            pthread_cond_broadcast(&elves_cond);
        }
        pthread_mutex_unlock(&elve_mutex);

        printf("SANTA CLAUSE IS GOING TO BED!\n");
    }

    exit(0);
}

void* elve_func(void* arg) {
    int id = *(int*) arg;

    while (1) {
        sleep(3);

        pthread_mutex_lock(&elve_mutex);
        while (elves_in_queue == ELVES_QUEUE_LEN) {
            printf("ELVE (%d) waiting for returning of triple \n", id);
            pthread_cond_wait(&elves_cond, &elve_mutex);
        }
        pthread_mutex_unlock(&elve_mutex);

        pthread_mutex_lock(&elve_mutex);
        if (elves_in_queue < ELVES_QUEUE_LEN) {
            // nowy elf w kolejce
            elves_queue[elves_in_queue] = id;
            elves_in_queue++;
            printf("There are %d elves waiting for santa, %d\n", elves_in_queue, id);
            if (elves_in_queue == ELVES_QUEUE_LEN) {
                printf("ELVE waking up santa, %d\n", id);
                pthread_mutex_lock(&santa_mutex);
                pthread_cond_broadcast(&santa_cond);
                pthread_mutex_unlock(&santa_mutex);
            }
        }
        pthread_mutex_unlock(&elve_mutex);
    }
}

int main() {
    pthread_t elves_threads[ELVES];
    pthread_t santa_thread;

    int elves_ids[ELVES];

    // init santa thread
    pthread_create(&santa_thread, NULL, santa_func, (void *) NULL);

    // init elves
    for (int i = 0; i < ELVES; i++) {
        elves_ids[i] = i;
        pthread_create(&elves_threads[i], NULL, elve_func, &elves_ids[i]);
    }

    pthread_join(santa_thread, NULL);
    for (int i = 0; i < ELVES; i++) {
        pthread_join(elves_threads[i], NULL);
    }


    pthread_mutex_destroy(&santa_mutex);
    pthread_mutex_destroy(&elve_mutex);
    pthread_cond_destroy(&santa_cond);
    pthread_cond_destroy(&elves_cond);
}
