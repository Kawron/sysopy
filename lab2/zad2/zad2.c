#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

int count_line(char line[], char* symb);
void count_lib(char* file_name, char* symb);
void count_sys(char* file_name, char* symb);

int main(int argc, char* argv[]) {
  char* symb = argv[1];
  char* file_name = argv[2];

  clock_t t;
  t = clock();
  count_lib(file_name,symb);
  t = clock() - t;
  printf("Lib time: %f\n", ((double)t)/CLOCKS_PER_SEC);
  t = clock();
  count_sys(file_name, symb);
  t = clock()-t;
  printf("Sys time: %f\n", ((double)t)/CLOCKS_PER_SEC);
}

void count_lib(char* file_name, char* symb) {
  FILE* file = fopen(file_name, "r");

  char line[256];
  int all = 0;
  int lines_count = 0;

  char c;
    int prev_lines_count = 0;
  while((c = fgetc(file)) != EOF) {
    if (c == symb[0]) all++;
    if (c == '\n') {
      if (prev_lines_count == all) continue;
      lines_count ++;
      prev_lines_count = all;
    }
  }
  printf("Lib res: all-%d lines-%d\n", all, lines_count);
  fclose(file);
}

void count_sys(char* file_name, char* symb) {
  int file = open(file_name, O_RDONLY);

  char c;
  int all = 0;
  int lines_count = 0;
  int prev_lines_count = 0;

  while(read(file, &c, 1) == 1) {
    if (c == symb[0]) all++;
    if (c == '\n') {
      if (prev_lines_count == all) continue;
      lines_count ++;
      prev_lines_count = all;
    }
  }
  printf("Lib res: all-%d lines-%d\n", all, lines_count);
  close(file);
}

// int count_line(char line[], char* symb) {
//   int size = 256;
//   int res = 0;
//   for (int i = 0; i < strlen(line); i++) {
//     if (line[i] == symb[0]) res++;
//   }
//   return res;
// }
