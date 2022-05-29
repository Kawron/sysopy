#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void thread_fun() {
  int* tmp = calloc(1,sizeof(int));
  pthread_setcancelstate(PTHREAD_CANCEL_DEFERRED, tmp);
  pthread_t n = pthread_self();
  int m = 0;
  while (1) {
    printf("START: Helllo, tu watek %ld, iteracja %d\n", n, m);
    pthread_testcancel();
    printf("END: Helllo, tu watek %ld, iteracja %d\n", n, m);
    m++;
  }
}

int main(int argc, char** argv) {


  int n = atoi(argv[1]);
  pthread_t** tmp = calloc(n,sizeof(pthread_t*));
  for (int i = 0; i < n; i++) {
    tmp[i] = calloc(1, sizeof(pthread_t));
  }

  for (int i = 0; i < n; i++) {
      pthread_create(tmp[i], NULL, &thread_fun, NULL);
  }
  int m = 0;
  while(m < 1000) {
    printf("Hello, tu watek glowny, iteracja %d\n", m);
    m++;
  }
  for (int i = 0; i < n; i++) {
    pthread_cancel(tmp[i]);
  }
  return 0;
}
