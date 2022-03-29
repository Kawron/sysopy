#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double calculate_res(int i, int n) {
  printf("n: %d\n", n);
  double width = (double) 1/n;
  printf("width: %f\n", width);
  double x1 = i * width;
  double x2 = (i+1) * width;
  double t = (x1+x2)/2;
  printf("x1: %f x2: %f t: %f\n", x1,x2,t);
  double val = 4/(pow(t,2)+1);
  printf("val: %f\n", val);
  double res = val*width;
  printf("res: %f\n", res);
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
  printf("main n: %d\n", n);

  double res = calculate_res(i,n);
  save_to_file(i, res);
}
