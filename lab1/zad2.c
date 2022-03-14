#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// #include <ctype.h>

/* Dobra lista argumentów to
--Size of Main array
--commends (e.g wc_files file1 file2 wc_files file3 file4)*/

/* dodać czyszczenie pamięci*/
int main(int argc, char* argv[]) {
  int i = 1;
  int j;
  int counter = 0;
  char** array;
  while (i < argc) {


    // printf("%s\n", argv[i]);
    if (strcmp(argv[i], "create_table") == 0) {
      //funckja is number
      if (is_number(argv[i+1]) == 0) {
        fprintf(stderr, "Argument of create_table must be a number");
        exit(1);
      }
      int size = atoi(argv[i+1]);
      create_table(size);
      i += 2;
    }

    else if (strcmp(argv[i], "wc_files") == 0) {
      // sprawdzic czy koniec
      j = i;
      counter = 0;
      while (j < argc) {
        j++;
        if (j < argc && strcmp(argv[j], "wc_files") != 0 && strcmp(argv[j], "create_table") != 0 && strcmp(argv[j], "remove_block") != 0) {
            counter += 1;
        }
        else break;
      }
      if (counter == 0) exit(1);
      array = calloc(counter, sizeof(char*));
      int x = 0;
      j = i;
      while (x < counter) {
        j++;
        array[x] = argv[j];
        x++;
      }
      int res =count(array, counter);
      i += counter + 1;
    }

    else if (strcmp(argv[i], "remove_block") == 0) {
      if (is_number(argv[i+1]) == 0) {
        fprintf(stderr, "Argument of remove_block must be a number");
        exit(1);
      }
      int index = atoi(argv[i+1]);
      remove_block(index);
      i += 2;
    }
  }
  free_memory();
  printf("DONE");
  return 0;
}

int is_number(char* word) {
  size_t size = strlen(word);
  // if (word[0] == '0' && size > 1) return 0;
  for (int i = 0; i < size; i++) {
    if (!isdigit(word[i])) {
      return 0;
    }
  }
  return 1;
}
