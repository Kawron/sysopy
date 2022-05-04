#include <stdio.h>
#include <sys/types.h>
#include <memory.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LINE_LENGHT 1000
#define MAX_DECLARATIONS 64
#define MAX_COMMANDS 100

int addedStatements = 0;
int commandsToExecute = 0;

char* deleteNewLine(char* word, int substitute) {
  int n = strlen(word);
  char* res = calloc(n, sizeof(char));
  for (int i = 0; i < n; i++) {
    res[i] = word[i];
    if (word[i] == '\n') {
      res[i] = substitute;
    }
  }
  return res;
}

char* deleteLastSpace(char* word, int substitute) {
  int n = strlen(word);
  char* res = calloc(n, sizeof(char));
  for (int i = 0; i < n; i++) {
    res[i] = word[i];
    if (word[i] == ' ' && i == n-1) {
      res[i] = substitute;
    }
  }
  return res;
}

char* deleteFirstSpace(char* word, int substitute) {
  int n = strlen(word);
  char* res = calloc(n, sizeof(char));
  for (int i = 0; i < n-1; i++) {
    res[i] = word[i+1];
  }
  return res;
}

void printCharByChar(char* word) {
  int i = 0;
  while(word[i] != '\0') {
    printf("%d ", word[i]);
    i++;
  }
  printf("\n");
}

char** seperateNameOptions(char* command) {
  char** res = calloc(4, sizeof(char*));
  for (int i = 0; i < 4; i++) {
    res[i] = calloc(100, sizeof(char));
  }
  int k = 0;
  char* token;
  token = strtok(command, " ");
  while (token != NULL) {
    strcpy(res[k], token);
    res[k] = deleteNewLine(res[k], 0);
    token = strtok(NULL, " ");
    k++;
  }
  res[k] = NULL;

  return res;
}


void executeCommands(char** commands) {

  int pipes[2][2];

  int i;
  for (i = 0; i < commandsToExecute; i++) {

    if (i > 0) {
      close(pipes[i % 2][0]);
      close(pipes[i % 2][1]);
    }

    pipe(pipes[i % 2]);

    pid_t pid = fork();

    if (pid < 0) {
      fprintf(stderr, "Cos nie dziala\n");
    }

    if (pid == 0) {

      char* res[100];
      char** tmp = calloc(100, sizeof(char*));
      for (int k = 0; k < 100; k++) {
        tmp[k] = calloc(100, sizeof(char));
      }
      int k = 0;
      char* token;
      token = strtok(commands[i], " ");
      while (token != NULL) {
        strcpy(tmp[k], token);
        tmp[k] = deleteNewLine(tmp[k], 0);
        token = strtok(NULL, " ");
        k++;
      }
      tmp[k] = NULL;
      int cnt = k;

      for (int k = 0; k < cnt; k++) {
        res[k] = tmp[k];
      }

      if ( i  !=  commandsToExecute-1) {
        close(pipes[i % 2][0]);
        dup2(pipes[i % 2][1], STDOUT_FILENO);
      }
      if (i != 0) {
        close(pipes[(i + 1) % 2][1]);
        dup2(pipes[(i + 1) % 2][0], STDIN_FILENO);
      }

      execvp(res[0], res);
      exit(1);
    }
  }
  close(pipes[i % 2][0]);
  close(pipes[i % 2][1]);
  for (int i = 0; i < commandsToExecute; ++i) {
    waitpid(-1,NULL,0);
  }
  wait(0);
  exit(0);
}

char* findStatement(char* line, char** statements) {
  char* token = calloc(100, sizeof(char));
  char* statementCopy = calloc(100, sizeof(char));
  char* res = calloc(100, sizeof(char));
  int flag = 0;

  for (int i = 0; i < addedStatements; i++) {
    strcpy(statementCopy, statements[i]);
    token = strtok(statementCopy, " = ");
    char* comparer = deleteNewLine(line, 0);

    if (strcmp(comparer, token) == 0) {
      token = strtok(NULL, "=");
      strcpy(res, token);

      flag = 1;
      free(comparer);
      break;
    }
  }
  free(statementCopy);
  if (flag == 1) return res;
  fprintf(stderr, "Nothing found\n");
  exit(1);
}

char** finalCommand(char** line, char** declarations, int cnt) {
  char** commands = calloc(MAX_COMMANDS, sizeof(char*));
  for (int i = 0; i < MAX_COMMANDS; i++) {
    commands[i] = calloc(MAX_LINE_LENGHT, sizeof(char));
  }

  char* tmp;
  for (int i = 0; i < cnt; i++) {
    tmp = findStatement(line[i], declarations);
    strcpy(commands[i], tmp);
  }

  //split
  char** res = calloc(MAX_COMMANDS, sizeof(char));
  for (int i = 0; i < MAX_COMMANDS; i++) {
    res[i] = calloc(MAX_LINE_LENGHT, sizeof(char));
  }
  int k = 0;
  for (int i = 0; i < cnt; i++) {
    char* token;
    token = strtok(commands[i], "|");
    while (token != NULL) {
      strcpy(res[k], token);
      token = strtok(NULL, "|");
      k++;
    }
  }
  commandsToExecute = k;

  return res;
}

void parse(char* line, char** declarations) {
  char** commands = calloc(MAX_COMMANDS, sizeof(char*));
  for (int i = 0; i < MAX_COMMANDS; i++) {
    commands[i] = calloc(MAX_LINE_LENGHT, sizeof(char));
  }

  int cnt = 0;
  char* token;
  token = strtok(line, " | ");
  while (token != NULL) {
    strcpy(commands[cnt], token);
    token = strtok(NULL, " | ");
    cnt++;
  }

  char** res = finalCommand(commands, declarations, cnt);

  for (int i = 0; i < commandsToExecute; i++) {
    res[i] = deleteNewLine(res[i], 0);
    res[i] = deleteLastSpace(res[i], 0);
    res[i] = deleteFirstSpace(res[i], 0);
  }

  // printf("ALL COMMANDS:\n");
  // for (int i = 0; i < commandsToExecute; i++) {
  //   printf("%s, ", res[i]);
  // }
  // printf("\n");
  //
  // printf("ALL COMMANDS CHAR BY CHAR:\n");
  // for (int i = 0; i < commandsToExecute; i++) {
  //   printCharByChar(res[i]);
  // }

  executeCommands(res);
}

int main(int argc, char* argv[]) {

  FILE* f = fopen(argv[1], "r");

  char** declarations = calloc(MAX_DECLARATIONS, sizeof(char*));
  for (int i = 0; i < MAX_DECLARATIONS; i++) {
    declarations[i] = calloc(MAX_LINE_LENGHT, sizeof(char));
  }

  int flag;
  int index = 0;
  char line[MAX_LINE_LENGHT];
  // that main loop looks good
  while(fgets(line, MAX_LINE_LENGHT, f) != NULL) {
    flag = 0;

    for (int i = 0; i < MAX_LINE_LENGHT; i++) {
      if (line[i] == '\n') break;
      if (line[i] == '\=') flag = 1;
    }

    // wykonac instrukcje
    if (flag == 0) {
      printf("Parsing line: %s\n", line);
      parse(line, declarations);
  }
    // definicja
    else {
      printf("Adding to statements line: %s\n", line);
      strcpy(declarations[index], line);
      addedStatements++;
      index++;
    }
  }
}
