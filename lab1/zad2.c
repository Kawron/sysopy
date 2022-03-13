#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Dobra lista argumentów to
--Size of Main array
--commends (e.g wc_files file1 file2 wc_files file3 file4)*/

int main(int argc, char* argv[]) {
  int i = 1;
  int j;
  int counter = 0;
  char** array;
  while (i < argc) {

    if (strcmp(argv[i], "create_table") == 0) {
      //funckja is number
      if (i+1 > argc) {exit(1);};
      int size = atoi(argv[i+1]);
      create_table(size);
    }

    else if (strcmp(argv[i], "wc_files") == 0) {
      printf("JAPIERDOLE\n");
      // sprawdzic czy koniec
      j = i;
      counter = 0;
      while (j < argc) {
        j++;
        if (j < argc && strcmp(argv[j], "wc_files") != 0 && strcmp(argv[j], "create_table") != 0) {
            counter += 1;
        }
        else break;
      }
      printf("no i kurwa wylaodowal\n");
      if (counter == 0) exit(1);
      printf("JAPIERDOLE\n");
      array = calloc(counter, sizeof(char*));
      int x = 0;
      j = i;
      while (x < counter) {
        j++;
        array[x] = argv[j];
        printf("%s ", array[x]);
        x++;
        // array[x] = calloc((int) sizeof(argv[j])/sizeof(char), sizeof(char));
        // strcat(array[x], argv[j]);
      }
      for (int k = 0; k < counter; k++) {
        printf("%s ", array[k]);
      }
      printf("JAPIERDOLE\n");
      int res =count(array, counter);
      // printf("%d\n", res);
    }
    i++;
  }
  return 0;
}
