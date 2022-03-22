#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
// #include "zad1.h"-

int count_line(char line[], char* symb);
void count_lib(char* file_name, char* symb);
void count_sys(char* file_name, char* symb);

int main(int argc, char* argv[]) {
  char* symb = argv[1];
  char* file_name = argv[2];

  count_lib(file_name,symb);
  count_sys(file_name, symb);
}

void count_lib(char* file_name, char* symb) {
  FILE* file = fopen(file_name, "r");

  char line[256];
  int all = 0;
  int lines_count = 0;

  // int x;
  // while (fgets(line, 256, file) != NULL) {
  //   x = count_line(line, symb);
  //   if (x > 0) {
  //     all += x;
  //     lines_count++;
  //   }
  // }
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
  printf("%d %d\n", all, lines_count);
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
  printf("%d %d\n", all, lines_count);
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
