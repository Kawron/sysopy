#ifdef NFTW
  #define _XOPEN_SOURCE 500
#endif
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

struct counterType {
  int file;
  int dir;
  int charDev;
  int blockDev;
  int fifo;
  int slink;
  int sock;
};

struct counterType globalCounter;

#ifdef STAT

char* getType(unsigned int type) {
  switch (type) {
    case DT_REG:
      globalCounter.file++;
      return "file";
    case DT_DIR:
      globalCounter.dir++;
      return "dir";
    case DT_CHR:
      globalCounter.charDev++;
      return "charDev";
    case DT_BLK:
      globalCounter.blockDev++;
      return "blockDev";
    case DT_FIFO:
      globalCounter.fifo++;
      return "fifo";
    case DT_LNK:
      globalCounter.slink++;
      return "slink";
    case DT_SOCK:
      globalCounter.sock++;
      return "sock";
    default:
      fprintf(stderr, "Can't find file type");
      exit(1);
  }
}

struct stat fileStats;

void recurr(char* dirPath) {
  char currPath[4096];
  DIR* dir = opendir(dirPath);
  if (dir == NULL) {
    return;
  }
  struct dirent* dirp;
  while ((dirp = readdir(dir)) != NULL) {
    if (strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..")) {
      snprintf(currPath, 4096*sizeof(char), "%s/%s", dirPath, dirp->d_name);
      char* fullPath = realpath(currPath, NULL);
      printf("Path: %s\n", fullPath);
      stat(dirp->d_name, &fileStats);
      printf("Links: %lu\n", fileStats.st_nlink);
      printf("Type: %s\n", getType(dirp->d_type));
      printf("Size: %lu\n", fileStats.st_size);
      char date[42];
      strftime(date, 42, "%d.%m.%Y %H:%M:%S", localtime(&fileStats.st_atime));
      printf("Last access time: %s\n", date);
      strftime(date, 42, "%d.%m.%Y %H:%M:%S", localtime(&fileStats.st_mtime));
      printf("Last modify time: %s\n", date);
      printf("---------------------------------------------------\n");
      recurr(currPath);
    }
  };
  closedir(dir);
}

#endif

#ifdef NFTW

char* getTypeNFTW(const struct stat* stats) {
  if (S_ISREG(stats->st_mode)) {
    globalCounter.file++;
    return "file";}
  if (S_ISDIR(stats->st_mode)) {
    globalCounter.dir++;
    return "dir";}
  if (S_ISCHR(stats->st_mode)) {
    globalCounter.charDev++;
    return "charDev";}
  if (S_ISBLK(stats->st_mode)) {
    globalCounter.blockDev++;
    return "blockDev";}
  if (S_ISFIFO(stats->st_mode)) {
    globalCounter.fifo++;
    return "fifo";}
  if (S_ISLNK(stats->st_mode)) {
    globalCounter.slink++;
    return "slink";}
  if (S_ISSOCK(stats->st_mode)) {
    globalCounter.sock++;
    return "sock";}
  fprintf(stderr, "Can't find file type");
  exit(1);
}

void getInfo(const char* path, const struct stat* fileStats, int limit, int flag) {
  char* fullPath = realpath(path, NULL);
  printf("Path: %s\n", fullPath);
  printf("Links: %lu\n", fileStats->st_nlink);
  printf("Type: %s\n", getTypeNFTW(fileStats));
  printf("Size: %lu\n", fileStats->st_size);
  char date[42];
  strftime(date, 42, "%d.%m.%Y %H:%M:%S", localtime(&fileStats->st_atime));
  printf("Last access time: %s\n", date);
  strftime(date, 42, "%d.%m.%Y %H:%M:%S", localtime(&fileStats->st_mtime));
  printf("Last modify time: %s\n", date);
}

#endif

void printInfo() {
  printf("Inforamtion about found files:\n");
  printf("Found: %d files, ", globalCounter.file);
  printf("%d directories, ", globalCounter.dir);
  printf("%d charDev, ", globalCounter.charDev);
  printf("%d blockDev, ", globalCounter.blockDev);
  printf("%d filo, ", globalCounter.fifo);
  printf("%d slink, ", globalCounter.slink);
  printf("%d sock, \n", globalCounter.sock);

}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Wrong number of arguments");
    exit(1);
  }
  #ifdef STAT
    recurr(argv[1]);
  #endif
  #ifdef NFTW
    int flags = FTW_PHYS;
    nftw(argv[1], getInfo, 64, flags);
    // bo nft zlicza też folder który jest podany jako argument
    globalCounter.directories -= 1;
  #endif
  printInfo();
  return 0;
}
