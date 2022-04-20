#include <stdlib.h>
#include <stdio.h>

int addedStatements = 0;
int commandsToExecute = 0;

void executeCommands(char** commands) {
  int** pipes = calloc(commandsToExecute, sizeof(int*));
  for (int i = 0; i < commandsToExecute; i++) {
    pipes[i] = calloc(2, sizeof(int));
  }

  for (int i = 0; i < commandsToExecute; i++) {
    pid_t pid = fork();
    if (pif == 0) {
      if (i > 0) {
        dup2(pipes[i-1][0], STDIN_FILENO);
      }
      if (i + 1 < commandsToExecute) {
        dup2(pipes[i][1], STDOUT_FILENO);
      }
      // to zmienic
      for (int j = 0; j < commandsToExecute -1; j++) {
        close(pipes[j][0]);
        close(pipes[j][1]);
      }
    }
  }
}

char** splitCommands(char** commands, int cnt) {
  commandsToExecute = 0;
  char** res = calloc(100, sizeof(char*));
  for (int i = 0; i < 100; i++) {
    res[i] = calloc(100, sizeof(char));
  }

  int index = 0;
  for (int i = 0; i < cnt; i++) {
    char* chunk = commands[i];
    printf("Chunk: %s\n", chunk);
    int k = 0;
    int flag = 0;
    while (1) {
      char* word = calloc(100,sizeof(char));
      int j = 0;

      while(1) {
        if (chunk[k] == '\0') {
          flag = 1;
          break;
        }
        if (chunk[k] == '|') {
          k+=2;
          break;
        }
        word[j] = chunk[k];
        j++;
        k++;
      }
      word[j] = '\0';
      res[index] = word;
      printf("Word: %s Index: %d\n", word,index);
      commandsToExecute++;
      index++;
      if (flag == 1) break;
    }
  }
  return res;
}

char* findInStatements(char* line, char** statements) {
  printf("In findInStatements\n");
  printf("Looking for %s\n", line);
  for (int i = 0; i < addedStatements; i++) {
    printf("Statement: %s\n", statements[i]);
    printf("Line: %s\n", line);
    int j = 0;
    int flag = 0;

    while(1) {
      printf("Comparing: %c to %c\n", statements[i][j],line[j]);
      if (statements[i][j] == ' ') {
        flag = 1;
        break;
      }
      if (line[j] != statements[i][j]) {
        break;
      }
      j++;
    }
    printf("Flag: %d\n", flag);
    if (flag == 0) continue;
    char* res = calloc(100, sizeof(char));
    int k = 0;
    j++;
    //skip " = "
    j += 2;
    while (statements[i][j] != '\n') {
      res[k] = statements[i][j];
      k++;
      j++;
    }
    res[k] == '\0';
    printf("RES: %s\n", res);
    return res;
    printf("exiting findInStatements\n");
  }
}

void clearBuffor(char* buffor) {
  for (int i = 0; i < 100; i++) {
    buffor[i] == "/0";
  }
}

void parse(char* line, char** statements) {
  char** commands = calloc(10, sizeof(char*));
  int cntCommnads = 0;
  for (int i = 0; i < 10; i++) {
    commands[i] = calloc(100, sizeof(char));
  }
  char* buffor = calloc(100, sizeof(char));
  clearBuffor(buffor);
  int i = 0;
  int j = 0;
  int k = 0;
  while (i < 100) {
    if (line[i] == '\n' || line[i] == EOF) {
      // add last line
      commands[j] = findInStatements(buffor, statements);
      cntCommnads++;
      clearBuffor(buffor);
      break;
    }
    else if (line[i] == '|') {
      // add commmand
      commands[j] = findInStatements(buffor, statements);
      cntCommnads++;
      clearBuffor(buffor);
      k = 0;
      // skip whitespace after |
      i++;
      j++;
    }
    else if (line[i] != " ") {
      buffor[k] = line[i];
      k++;
    }
    i++;
  }
  printf("-----------------\n");
  for (int i = 0; i < cntCommnads; i++) {
    printf("%s |", commands[i]);
  }
  printf("\n-----------------\n");
  char** res = splitCommands(commands, cntCommnads);
  // executeCommands(commands, cntCommnads);
}

int main(int argc, char* argv[]) {

  if (argc <= 1) {
    fprintf(stderr, "Too litle arguments\n");
  }

  int flag;
  int maxLineLength = 126;

  FILE* f = fopen(argv[1], "r");

  char** statements = calloc(200, sizeof(char*));
  for (int i = 0; i < 200; i++) {
    statements[i] = calloc(maxLineLength, sizeof(char));
  }
  int index = 0;

  char* line = calloc(maxLineLength, sizeof(char));
  while(fgets(line, maxLineLength, f) != NULL) {
    flag = 0;
    // fgets(line, maxLineLength, f);

    for (int i = 0; i < maxLineLength; i++) {
      if (line[i] == '\n') break;
      if (line[i] == '\=') flag = 1;
    }

    // wykonac instrukcje
    if (flag == 0) {
      printf("Parsing line: %s\n", line);
      parse(line, statements);
    // definicja
  }
    else {
      printf("adding to statements line: %s\n", line);
      strcpy(statements[index], line);
      // statements[index] = line;
      addedStatements++;
      index++;
    }
  }
}
