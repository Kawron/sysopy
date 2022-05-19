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

    int pizza_number;
    int pizza_idx;

    while(1) {

        struct shared_memory* shared_mem = shmat(sh_memory_id, NULL, 0);

        pizza_number = rand() % 10;

        get_time(&s, &ms);

        printf("pid:%d %ld.%04ld> COOK: making pizza no. %d\n", pid, s, ms, pizza_number);
        sleep(2);

        semop(semaphore_id, &mem_lock, 1);
        semop(semaphore_id, &put_pizza_in_oven, 1);

        pizza_idx = shared_mem->oven_index % OVEN_CAPACITY;
        shared_mem->oven[pizza_idx] = pizza_number;
        shared_mem->num_of_pizzas_oven++;

        get_time(&s, &ms);
        printf("pid:%d %ld.%04ld> COOK: put pizza no. %d, pizzas in oven: %d\n",
               pid, s, ms, pizza_number,
               shared_mem->num_of_pizzas_oven);

        semop(semaphore_id, &mem_unlock, 1);

        sleep(4);

        semop(semaphore_id, &get_pizza_from_oven, 1);
        semop(semaphore_id, &put_pizza_on_the_table, 1);
        semop(semaphore_id, &add_pizza_to_deliver, 1);
        semop(semaphore_id, &mem_lock, 1);

        shared_mem->oven[pizza_idx] = -1;
        shared_mem->oven_index++;
        shared_mem->num_of_pizzas_oven--;
        shared_mem->table[shared_mem->table_index % OVEN_CAPACITY] = pizza_number;
        shared_mem->num_of_pizzas_table++;
        get_time(&s, &ms);
        printf("pid:%d %ld.%04ld> COOK:taking out pizza no. %d, pizzas in oven: %d, pizzas on table: %d\n",
               pid, s, ms, pizza_number,
               shared_mem->num_of_pizzas_oven,
               shared_mem->num_of_pizzas_table);

        semop(semaphore_id, &mem_unlock, 1);
        shmdt(shared_mem);
    }
}
