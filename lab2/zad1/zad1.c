#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "zad1.h"

int main(int argc, char* argv[]) {
  char* file_from;
  char* file_to;
  if (argc < 3) {
    file_from = calloc(256, sizeof(char));
    file_to = calloc(256, sizeof(char));

    printf("Podaj nazwę pierwszego pliku: ");
    scanf("%s", file_from);
    printf("Podaj nazwę drugiego pliku: ");
    scanf("%s", file_to);
  }
  else {
    file_from = argv[1];
    file_to = argv[2];
  }
  copy_lib(file_from, file_to);
}

void copy_lib(char* file_from, char* file_to) {
  char c;
  FILE* from = fopen(file_from, "r");
  FILE* to = fopen(file_to, "w");

  while((c = fgetc(from)) != EOF) {
    if (c >= 33 && c <= 126) {
      fputc(c, to);
    }
  }
  fclose(from);
  fclose(to);
}

void copy_sys(char* file_from, char* file_to) {
  char c;
  int from, to;
  from = open(file_from, O_RDONLY);
  to = open(file_to, O_WRONLY);

  while (read(from, &c, 1) == 1) {
    if (c >= 33 && c <= 126) {
      write(to, &c, 1);
    }
  }
}
