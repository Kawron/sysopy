#include "zad1.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char** global_pointer = NULL;
int array_size;

void create_table(int size) {
  if (global_pointer != NULL) {
    fprintf(stderr, "Table of pointers already created\n");
    exit(1);
  }
  /*# wieksza niz int*/
  if (size < 0) {
    fprintf(stderr, "Incorrect size of table");
    exit(1);
  }
  char** block_array = calloc(size, sizeof(char*));
  global_pointer = block_array;
  array_size = size;
  return;
}

int count(char** files, int size) {
  char files_names[100000];
  for (int i = 0; i < size; i++) {
    strcat(files_names, files[i]);
    strcat(files_names, " ");
  }
  char buffer[100000];
  snprintf(buffer, sizeof(buffer), "wc %s > temp.txt", files_names);

   system(buffer);

  FILE* file;
  file = fopen("temp.txt", "r");
  fseek(file, 0L, SEEK_END);
  int len = ftell(file);
  fseek(file, 0L, SEEK_SET);

  int index = find_free_block();
  /*czy to powinno byc tu*/
  if (index == -1) {
    fprintf(stderr, "Didn't find free block of memory");
    exit(1);
  }

  char* ptr = calloc(len+1, sizeof(char));
  global_pointer[index] = ptr;
  int res = fread(ptr , 1, len , file);

  close(file);
  return index;
}

int find_free_block() {
  for (int i = 0; i < array_size; i++) {
    if (global_pointer[i] == NULL) return i;
  }
  return -1;
}

void remove_block(int index) {
  char* ptr = global_pointer[index];
  free(ptr);
  global_pointer[index] = NULL;
}

void free_memory() {
  for (int i = 0; i < array_size; i++) {
    if (global_pointer[i] != NULL) free(global_pointer[i]);
  }
  free(global_pointer);
}
