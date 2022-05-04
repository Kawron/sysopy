#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_SIZE 126

void send(char* address, char* subject, char* body) {
    char* cmd = calloc(MAX_LINE_SIZE, sizeof(char));

    sprintf(cmd, "echo %s | mail -s %s %s", body, subject, address);

    FILE* file = popen(cmd, "r");

    free(cmd);
}

void sort(char* flag) {
    char* cmd = calloc(MAX_LINE_SIZE, sizeof(char));

    if (strcmp(flag,"date") == 0) {
      cmd = "echo | mail -f | tail +2 | head -n-2 | cut -d' ' -f3- | awk '{if(!($3==\"Delivery\")) print}' | sort -k6";
    }
    if (strcmp(flag,"nadawca") == 0) {
      cmd = "echo | mail -f | tail +2 | head -n-2 | cut -d' ' -f3- | awk '{if(!($3==\"Delivery\")) print}' | sort -k6";
    }

    FILE* f = popen(cmd,"r");
    char* line = calloc(MAX_LINE_SIZE, sizeof(char));

    size_t size = 0;
    while (getline(&line, &size, f) != -1) {
      printf("%s", line);
    }

    pclose(f);
    free(line);
}

int main(int argc, char **argv) {

    if (argc == 2) {
      sort(argv[1]);
    }
    else if (argc == 4) {
      send(argv[1], argv[2], argv[3]);
    }
    else {
      fprintf(stderr, "Wrong number of args\n");
    }

    return 0;
}
