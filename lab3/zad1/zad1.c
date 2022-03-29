#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  int n = atoi(argv[1]);

  pid_t child_pid;

  for (int  i = 0; i < n; i++) {
    child_pid = fork();
    if (child_pid == -1) {
      fprintf(stderr, "Nie utworzono procesu dziecka\n");
      exit(1);
    }
    if (child_pid == 0) {
      printf("Jestem dzieckiem o pid: %d\n", (int)getpid());
      exit(0);
    }
  }
}
