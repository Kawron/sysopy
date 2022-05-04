#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>

#include "definitions.h"

int queue_id;
int local_id;
key_t local_key;

void stop() {
  struct message msg;
  msg.sender = local_key;
  msg.mtype = 0;
  msg.type = STOP;

  if (msgsnd(queue_id, &msg, MSG_SIZE, 0) == -1) {
    fprintf(stderr, "CLIENT: couldn't send stop message\n");
    exit(1);
  }
  msgctl(local_id, IPC_RMID, NULL);
}

void subscribe() {
  struct message msg;
  msg.mtype = 0;
  // msg.type = SUBSCRIBE;
  msg.sender = local_key;
  char test[10] = "chuj";
  sprintf(msg.msg_text, "%s", test);
  // msg.msg_text = "";
  printf("%s\n", msg.msg_text);
  printf("%d\n", msg.mtype);
  printf("%d\n", msg.type);
  printf("%d\n", msg.sender);
  printf("%d\n", queue_id);
  printf("%s\n", msg.msg_text);
  if (msgsnd(queue_id, &msg, MSG_SIZE, 0) == -1) {
    if (errno == EINVAL) printf("Ja pierdole no\n");
    fprintf(stderr, "CLIENT: subscribe request failed\n");
    exit(1);
  }
  if (msgrcv(local_id, &msg, MSG_SIZE, 0,0) == -1) {
    fprintf(stderr, "CLIENT: didn't receive response\n");
    exit(1);
  }
  printf("CLIENT with pid %d registered\n", getpid());
}

void get_list() {
  struct message msg;
  msg.sender = local_key;
  msg.mtype = 2;
  msg.type = GET_LIST;

  if (msgsnd(queue_id, &msg, MSG_SIZE, 0) == -1) {
    fprintf(stderr, "CLIENT: sending listing request failed\n");
    exit(1);
  }
  if (msgrcv(local_id, &msg, MSG_SIZE, 0, 0) == -1) {
    fprintf(stderr, "CLIENT: didn't receive listing response\n");
    exit(1);
  }
  printf("CLIENT: \n");
  printf("%s\n", msg.msg_text);
}

void send_2all() {
  struct message msg;
  msg.sender = local_key;
  msg.mtype = 3;
  msg.type = SEND_2ALL;

  printf("CLIENT: enter your message: ");
  if (fgets(msg.msg_text, MSG_SIZE, stdin) == -1) {
    printf("CLIENT: something went wrong try again\n");
    return;
  }
  printf("\n");

  if (msgsnd(queue_id, &msg, MSG_SIZE, 0) == -1) {
    fprintf(stderr, "CLIENT: sending msg to all failed\n");
    exit(1);
  }
}

void send_2one() {
  struct message msg;
  msg.sender = local_key;
  msg.mtype = 4;
  msg.type = SEND_2ONE;

  printf("CLIENT: enter your message: ");
  if (fgets(msg.msg_text, MSG_SIZE, stdin) == -1) {
    printf("CLIENT: something went wrong try again\n");
    return;
  }
  printf("\n");

  if (msgsnd(queue_id, &msg, MSG_SIZE, 0) == -1) {
    fprintf(stderr, "CLIENT: sending msg to all failed\n");
    exit(1);
  }
}

int main(int argc, char** argv) {
  char* home = getenv("HOME");

  key_t queue_key = ftok(home, PROJECT_ID);
  if (queue_key == -1) {
    fprintf(stderr, "Wywalilo sie\n");
    exit(1);
  }

  queue_id = msgget(queue_key, 0);
  if (queue_id == -1) {
    fprintf(stderr, "Wywalilo sie\n");
    exit(1);
  }

  local_key = ftok(home, getpid());
  if (local_key == -1) {
    fprintf(stderr, "Wywalilo sie\n");
    exit(1);
  }

  local_id = msgget(local_key, IPC_CREAT | IPC_EXCL | 0666);
  if (local_id == -1) {
    fprintf(stderr, "Wywalilo sie\n");
    exit(1);
  }

  subscribe();

  while(1) {
    char text[64];
    printf("CLIENT: enter your message: ");
    if (fgets(text, 64, stdin) == NULL) {
      printf("CLIENT: something webt wrong try again\n");
      continue;
    }
    printf("\n");
    printf("CLIENT: msg is \"%s\"\n", text);

    if (strcmp(text, "LIST") == 0) {
      get_list();
    }
    if (strcmp(text, "2ALL") == 0) {
      send_2all();
    }
    if (strcmp(text, "2ONE") == 0) {
      send_2one();
    }
    if (strcmp(text, "STOP") == 0) {
      stop();
      printf("CLIENT: ended client work\n");
      exit(0);
    }
  }
}
