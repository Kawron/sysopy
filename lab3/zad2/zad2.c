#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


int main(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Too litle arguments\n");
    exit(1);
  }
  int n = atoi(argv[1]);
  pid_t child_pid;
  for (int i = 0; i < n; i++) {
    child_pid = fork();
    if (child_pid == -1) {
      fprintf(stderr, "Couldn't fork\n");
      exit(1);
    }
    char opt1[12];
    char opt2[12];
    sprintf(opt1, "%d", i);
    sprintf(opt2, "%d", n);
    printf("%s %s\n", opt1, opt2);
    if (child_pid == 0) {
      // inside child process

      int err = execl("./calculate", "main", opt1, opt2, NULL);
      // int err = exec("ls",NULL);

      if (err == -1) {
        fprintf(stderr, "execl didn't work\n");
        exit(1);
      }
      exit(0);
    }
  }
  if (child_pid != 0) {
    // inside parent process
    waitpid(-1, NULL, 0);

  }
}
