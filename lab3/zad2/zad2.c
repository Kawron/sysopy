#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/times.h>
#include <time.h>

clock_t clock_begin, clock_end;
struct tms start, end;

void start_time() {
  clock_begin = times(&start);
}

void stop_time() {
  clock_end = times(&end);
}

void printTime() {
  int64_t clock_tck = sysconf(_SC_CLK_TCK);

  double time = (double)(clock_end - clock_begin) / clock_tck;
  printf("time: %.6f\n", time);
}

int main(int argc, char* argv[]) {
  start_time();
  if (argc < 2) {
    fprintf(stderr, "Too litle arguments\n");
    exit(1);
  }
  int n = atoi(argv[1]);
  pid_t child_pid;
  for (int i = 0; i < n; i++) {
    char opt1[12];
    char opt2[12];
    sprintf(opt1, "%d", i);
    sprintf(opt2, "%d", n);
    child_pid = fork();
    if (child_pid == -1) {
      fprintf(stderr, "Couldn't fork\n");
      exit(1);
    }
    if (child_pid == 0) {
      // inside child process
      int err = execl("./calc", "calc", opt1, opt2, NULL);
      if (err == -1) {
        fprintf(stderr, "execl didn't work\n");
        exit(1);
      }
      exit(0);
    }
  }
  if (child_pid != 0) {
    // inside parent process
    double res = 0;
    // to z jakiegos powodu nie dziala
    // waitpid(-1, NULL, 0);
    while(wait(NULL) > 0);
    for (int i = 0; i < n; i++) {
      char* buffor = calloc(12,sizeof(char));
      sprintf(buffor, "w%d.txt", i);

      FILE* f = fopen(buffor, "r");
      if (f == NULL) fprintf(stderr, "Something wrong cannot find w%d.txt\n", i);

      char* fileRes = calloc(16, sizeof(char));
      fread(fileRes, sizeof(float), 1, f);
      res += (double) atof(fileRes);
      fclose(f);
      free(buffor);
    }
    printf("res: %.12f\n", res);
  }
  stop_time();
  printTime();
}
