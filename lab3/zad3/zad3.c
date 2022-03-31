#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>


void readFile(char* fullPath, char* relPath, char* string) {

  FILE* f = fopen(fullPath, "r");
  if (f == NULL) {
    fprintf(stderr, "Can't open file: %s\n", fullPath);
    exit(1);
  }

  fseek(f, 0, SEEK_END);
  int size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char* buffor = calloc(size, sizeof(char));

  fread(buffor, sizeof(char), size, f);

  int ret = 0;
  int pid = getpid();
  if (strstr(buffor, string) != NULL) {
    printf("Path to file: %s, found by procces with %d \n", relPath, pid);
  }

  fclose(f);
  free(buffor);
}

void recurr(char *path, char* string) {

  DIR* dir = opendir(path);
  if (dir == NULL) {
    fprintf(stderr, "Couldn't open directory\n");
    exit(1);
  }
  struct dirent* dirp;

  char* currPath = calloc(4096, sizeof(char));
  while ((dirp = readdir(dir)) != NULL) {
    if (strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..")) {

      snprintf(currPath, 4096*sizeof(char), "%s/%s", path, dirp->d_name);
      char* fullPath = realpath(currPath, NULL);

      unsigned int type = dirp->d_type;
      if (type == DT_DIR) {
        // tu wchodzimy
        pid_t child_pid;
        child_pid = fork();

        if (child_pid == 0) {
          // child process
          recurr(currPath, string);
          exit(0);
        }
      }
      if (type == DT_REG) {
        readFile(fullPath, currPath, string);
      }
      free(fullPath);
    }
  }
  free(currPath);
  closedir(dir);
  while(wait(NULL) > 0);
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Too litle arguments\n");
    exit(1);
  }
  char* path = argv[1];
  char* string = argv[2];
  recurr(path, string);
}
