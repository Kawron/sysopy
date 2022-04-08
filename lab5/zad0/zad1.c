#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

  pid_t child_pid;
  int fd[2];
  pipe(fd);
  char msg[] = "Hello World";
  int n = strlen(msg)+1;
  // printf("%s\n", msg);

  child_pid = fork();
  if (child_pid == 0) {
    // child process
    printf("Child process withc PID: %d\n", (int)getpid());
    close(fd[1]);
    char buffor[n];
    read(fd[0], &buffor, n);
    printf("%s\n", buffor);
    exit(0);
  }
  close(fd[0]);
  write(fd[1], msg, n);
}
