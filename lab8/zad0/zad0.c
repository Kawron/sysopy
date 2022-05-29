#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void thread_fun() {
  pthread_t n = pthread_self();
  int m = 0;
  while (1) {
    printf("Helllo, tu watek %ld, iteracja %d\n", n, m);
    m++;
  }
}

int main(int argc, char** argv) {

  pthread_t* tmp = calloc(1,sizeof(pthread_t));

  int n = atoi(argv[1]);
  for (int i = 0; i < n; i++) {
      pthread_create(tmp, NULL, &thread_fun, NULL);
  }
  int m = 0;
  while(1) {
    printf("Hello, tu watek glowny, iteracja %d\n", m);
  }
  return 0;
  printf("TEST\n");
}
