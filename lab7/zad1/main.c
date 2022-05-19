#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <wait.h>
#include <string.h>
#include <errno.h>

#include "symaphors.h"

int semaphore_id;
int shared_memory_id;
int num_of_workers;
pid_t* children;

void open_semaphores() {
    key_t key = ftok(getenv("HOME"), PROJECT_ID);
    semaphore_id = semget(key, 4, IPC_CREAT | PERMISSIONS);

    semctl(semaphore_id, OVEN_SEMAPHOR_LOCK, SETVAL, OVEN_CAPACITY);
    semctl(semaphore_id, TABLE_SEMPAHOR_LOCK, SETVAL, TABLE_CAPACITY);
    semctl(semaphore_id, DELIVER_SYMAPHOR_LOCK, SETVAL, 0);
    semctl(semaphore_id, UNIVERSAL_LOCK, SETVAL, 1);

    key = ftok(getenv("HOME"), PROJECT_ID_REVERSE);
    shared_memory_id = shmget(key, sizeof (struct shared_memory), PERMISSIONS | IPC_CREAT);
}

void clean_memory() {
    for(int i = 0; i < num_of_workers; i++) {
        kill(children[i], SIGINT);
    }

    if(shared_memory_id != -1){
        shmctl(shared_memory_id, IPC_RMID, 0);
    }

    if(semaphore_id != -1){
        semctl(semaphore_id, 0, IPC_RMID);
    }
}

int main(int argc, char** argv) {

    if (argc != 3) {
      fprintf(stderr, "Not enough argumetns");
      exit(1);
    }

    signal(SIGINT, clean_memory);

    int cooks = atoi(argv[1]);
    int delivers = atoi(argv[2]);
    num_of_workers = cooks + delivers;

    open_semaphores();

    children = calloc(num_of_workers, sizeof(pid_t));

    int idx = 0;
    pid_t pid;
    for (int i = 0; i < cooks; ++i) {
        pid = fork();
        if (pid == 0){
            execl("./cook", "./cook", NULL);
            return 0;
        }
        children[idx] = pid;
        idx++;
    }
    for (int i = 0; i < delivers; ++i) {
        pid = fork();
        if (pid == 0){
            execl("./currier", "./currier", NULL);
            return 0;
        }
        children[idx] = pid;
        idx++;
    }

    for (int i = 0; i < num_of_workers; ++i) {
        wait(0);
    }

    clean_memory();
    return 0;
}
