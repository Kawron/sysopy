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
#include <errno.h>.
#include "definitions.h"


key_t clients_key[MAX_CLIENTS_NUMBER];
key_t server_key;
int server_id = 0;
int clients_number = 0;

void register_user(struct message* msg) {
  int user_key = atoi(msg->sender);
  int flag = 0;
  for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
    if (clients_key[i] == -1) {
      clients_key[i] == user_key;
      flag = 1;
      break;
    }
  }
  if (flag == 0) {
    printf("SERVER: not enough place for more users\n");
    return;
  }
  int client_queue_id = msgget(user_key, 0);
  msg->type = INIT;
  msg->sender = server_key;
  msgsnd(client_queue_id, &msg, MAX_MSG_SIZE, 0);
  clients_number++;
}

void listing_request(struct message* msg) {
  int user_key = msg->sender;
  int client_queue_id = msgget(user_key, 0);

  char buffor[MAX_MSG_SIZE+1];
  int idx = 0;
  for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
    if (clients_key[i] != -1) {
      idx += sprintf(&buffor[idx], "%d", clients_key[i]);
    }
  }
  buffor[idx] = 0;
  strcpy(msg->msg_text, buffor);
  msg->sender = server_key;
  msgsnd(client_queue_id, &msg, MAX_MSG_SIZE, 0);
}

void user_stoped(struct message* msg) {
  int user_key = msg->sender;

  for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
    if (clients_key[i] == user_key) {
      clients_key[i] = -1;
      clients_number--;
      return;
    }
  }
  fprintf(stderr, "SERVER: couldn't delete user\n");
  exit(1);
}

void response(struct message* msg) {
  switch (msg->type) {
    case SUBSCRIBE:
      register_user(msg); //done
    case STOP:
      user_stoped(msg); //done
      break;
    case GET_LIST:
      listing_request(msg); //done
      break;
    // case SEND_2ALL:
    //   user_send_2all(msg);
    //   break;
    // case SEND_2ONE:
    //   user_send_2one(msg);
    //   break;
    default:
      break;
  }
}

// void send_shutdown() {
//   message msg;
//   msg.sender = server_key;
//   msg.type
// }

void close_server() {
  msgctl(server_id, IPC_RMID, NULL);
}

void SIGINT_handler(int signal_num) {
  // send_shutdown();
  close_server();
}

int main(int argc, char** argv) {
  char* home = getenv("HOME");

  for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
      clients_key[i] = -1;
  }

  server_key = ftok(home, PROJECT_ID);
  if (server_key == (key_t)-1) {
    fprintf(stderr, "SERVER: server key is: %d\n");
    exit(1);
  }

  server_id = msgget(server_key, IPC_CREAT);
  if (server_id == -1) {
    fprintf(stderr, "Wywalilo sie\n");
    exit(1);
  }

  struct sigaction action;

  action.sa_handler = SIGINT_handler;

  sigemptyset(&action.sa_mask);
  sigaddset(&action.sa_mask, SIGINT);
  action.sa_flags = 0;
  sigaction(SIGINT, &action, NULL);

  struct message msg;

  while(1) {
    printf("%d\n", server_id);
    printf("%d\n", server_key);
    if (msgrcv(server_id, &msg, sizeof(struct message), -64,0) == -1) {
      if (errno == EACCES) printf("DUPA\n");
      fprintf(stderr, "SERVER: couldn't receive msg\n");
      exit(1);
    }
    else {
      response(&msg);
    }
  }
}
