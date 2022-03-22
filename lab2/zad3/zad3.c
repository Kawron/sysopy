#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>


int main(int argc, char* argv[]) {
  recurr(argv[1]);
}

void recurr(char* dirPath) {
  char* currPath = calloc(4096, sizeof(char));
  DIR* dir = opendir(dirPath);
  struct dirent* dirp;
  while ((dirp = readdir(dir)) != NULL) {
    // char fileName[] = dirp->d_name;
    if (strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..")) {
      printf("%s\n", dirp->d_name);
      snprintf(currPath, 4096*sizeof(char), "%s/%s", dirPath, dirp->d_name);
      printf("%s\n", currPath);
      currPath = realpath(currPath, NULL);

      struct stat fileStats;

      stat(dirp->d_name, &fileStats);

      printf("Path: %s\n", currPath);
    }
  }
}
