#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double calculate_res(int i, int n) {
  double width = (double) 1/n;
  double x1 = i * width;
  double x2 = (i+1) * width;
  double t = (x1+x2)/2;
  double val = 4/(pow(t,2)+1);
  double res = val*width;
  return res;
}

void save_to_file(int i, double res) {
  char* buffor = calloc(sizeof(char), 12);
  sprintf(buffor, "w%d.txt", i);
  FILE* f = fopen(buffor, "w+");
  fprintf(f, "%.12f", res);
  fclose(f);
  free(buffor);
}
// argumenty:
// i-numer procesu (zaczynając od 0)
// n-ilość podziałów
int main(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Too litle arguments\n");
    exit(1);
  }
  int i = atoi(argv[1]);
  int n = atoi(argv[2]);

  double res = calculate_res(i,n);
  save_to_file(i, res);
}
