#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
// #include "zad1.h"-
// #include <cstdio>

int main(int argc, char* argv[]) {
  char* symb = argv[1];
  char* file_name = argv[2];

}

void count_lib(char* file_name, char symb) {

}

// int count_sys(char* file_name, char* symb) {
//   char c;
//   int file = open(file_name, O_RDONLY);
//
//   int all = 0;
//   int in_line = 0;
//   int last_line = 0;
//
//   while (read(file, &c, 1) == 1) {
//     if (c == symb) all++;
//     if (c == '\n') {
//       if (last_line != all) in_line++;
//       last_line = all;
//     }
//   }
//   // int* res = calloc(2, sizeof(int));
//   // res[0] = all;
//   // res[1] = in_line;
//   printf("%d\n", all);
//   printf("%d\n", in_line);
//   return 1;
// }
