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

#define PROJECT_ID 1234
#define PROJECT_ID_REVERSE 9876
#define PERMISSIONS 0666
#define OVEN_CAPACITY 5
#define TABLE_CAPACITY 5

#define OVEN_SEMAPHOR_LOCK 0
#define TABLE_SEMPAHOR_LOCK 1
#define DELIVER_SYMAPHOR_LOCK 2
#define UNIVERSAL_LOCK 3

struct sembuf put_pizza_in_oven = {OVEN_SEMAPHOR_LOCK, -1, 0};
struct sembuf get_pizza_from_oven = {OVEN_SEMAPHOR_LOCK, 1, 0};
struct sembuf put_pizza_on_the_table = {TABLE_SEMPAHOR_LOCK, -1, 0};
struct sembuf get_pizza_from_table = {TABLE_SEMPAHOR_LOCK, 1, 0};
struct sembuf deliver_pizza = {DELIVER_SYMAPHOR_LOCK, -1, 0};
struct sembuf add_pizza_to_deliver = {DELIVER_SYMAPHOR_LOCK, 1, 0};

struct sembuf mem_lock = {UNIVERSAL_LOCK, -1, 0};
struct sembuf mem_unlock = {UNIVERSAL_LOCK, 1, IPC_NOWAIT};

struct shared_memory{
    int num_of_pizzas_oven;
    int oven_index;
    int num_of_pizzas_table;
    int table_index;

    int oven[OVEN_CAPACITY];
    int table[TABLE_CAPACITY];
};

void get_time(long *s, long *ms)
{
    struct timespec time;

    clock_gettime(CLOCK_REALTIME, &time);

    *s  = time.tv_sec;
    *ms = round(time.tv_nsec / 1.0e6);
    if (*ms > 999) {
        (*s)++;
        *ms = 0;
    }
}
