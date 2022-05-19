#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>

#include "symaphors.h"

long s;
long ms;

int main(int argc, char** argv) {

    pid_t pid = getpid();

    srand(pid);

    key_t key = ftok(getenv("HOME"), PROJECT_ID);
    int semaphore_id = semget(key, 0, PERMISSIONS);

    key = ftok(getenv("HOME"), PROJECT_ID_REVERSE);
    int sh_memory_id = shmget(key, 0, PERMISSIONS);

    int pizza_type;
    int pizza_idx;

    while(1) {
        struct shared_memory* shared_mem = shmat(sh_memory_id, NULL, 0);

        semop(semaphore_id, &deliver_pizza, 1);
        semop(semaphore_id, &get_pizza_from_table, 1);
        semop(semaphore_id, &mem_lock, 1);

        pizza_idx = shared_mem->table_index % TABLE_CAPACITY;

        pizza_type = shared_mem->table[pizza_idx];
        shared_mem->table[pizza_idx] = pizza_type;
        shared_mem->num_of_pizzas_table--;
        shared_mem->table_index++;
        get_time(&s, &ms);
        printf("pid:%d %ld.%04ld> CURRIER: taking pizza no. %d, pizzas on table: %d\n",
               pid, s, ms, pizza_type,
               shared_mem->num_of_pizzas_table);
        semop(semaphore_id, &mem_unlock, 1);
        sleep(4);

        get_time(&s, &ms);
        printf("pid:%d %ld.%04ld> CURRIER: delivering pizza no. %d\n",
               pid, s, ms, pizza_type);

        sleep(4);
        get_time(&s, &ms);
        printf("pid:%d %ld.%04ld> CURRIER: pizza delivered\n", pid, s, ms);

        shmdt(shared_mem);
    }
}
