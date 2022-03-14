#include "zad1.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Block** global_pointer = NULL;
int array_size;

struct Block {
  char* line;
};

void create_table(int size) {
  if (global_pointer != NULL) {
    fprintf(stderr, "Table of pointers already created\n");
    return;
  }
  /*# wieksza niz int*/
  if (size < 0) {
    fprintf(stderr, "Incorrect size of table");
    return;
  }
  struct Block** block_array = calloc(size, sizeof(struct Block*));
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
    return -1;
  }

  struct Block* ptr = calloc(1, sizeof(struct Block));
  global_pointer[index] = ptr;
  ptr->line = calloc(len+1,sizeof(char));
  char* lineptr = ptr->line;
  int res = fread(lineptr , 1, len , file);

  for (int i = 0; i < len; i++) {
    printf("%c", lineptr[i]);
  }

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
  struct Block* ptr = global_pointer[index];
  char* lineptr = ptr->line;
  free(lineptr), free(ptr);
}
