#define _GNU_SOURCE


#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
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
#include <errno.h>
#include "definitions.h"


key_t clients_id[MAX_CLIENTS_NUMBER] = {-1};
key_t server_key;
int server_id = 0;
int clients_number = 0;
struct msg user_request;
struct msg server_response;

void register_user() {
  int user_id = user_request.msg_text.sender;
  int flag = 0;
  for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
    if (clients_id[i] == -1) {
      clients_id[i] = user_id;
      flag = 1;
      break;
    }
  }
  if (flag == 0) {
    printf("SERVER: not enough place for more users\n");
    return;
  }
  server_response.msg_type = 5;
  server_response.msg_text.sender = server_id;

  msgsnd(user_id, &server_response, sizeof(struct msg_text), 0);
  clients_number++;
}

void listing_request() {
  int user_id = user_request.msg_text.sender;

  char buffor[sizeof(struct msg_text)];
  int idx = 0;
  for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
    if (clients_id[i] != -1) {
      idx += sprintf(&buffor[idx], "%d\n", clients_id[i]);
    }
  }
  buffor[idx] = 0;
  strcpy(server_response.msg_text.buf, buffor);
  server_response.msg_text.sender = server_id;
  msgsnd(user_id, &server_response, sizeof(struct msg_text), 0);
}

void send_2all() {
  int user_id = user_request.msg_text.sender;

  server_response.msg_type = _2ALL;
  server_response.msg_text.sender = user_id;
  strcpy(server_response.msg_text.buf, user_request.msg_text.buf);

  for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
    msgsnd(clients_id[i], &server_response, sizeof(struct msg_text), 0);
  }
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

void response() {
  switch (user_request.msg_type) {
    case INIT:
      register_user(); //done
      break;
    // case STOP:
    //   user_stoped(msg); //done
    //   break;
    case GET_LIST:
      listing_request(); //done
      break;
    case SEND_2ALL:
      user_send_2all(msg);
      break;
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

// void SIGINT_handler(int signal_num) {
//   // send_shutdown();
//   close_server();
// }

int main(int argc, char** argv) {
  char* home = getenv("HOME");

  for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
      clients_id[i] = -1;
  }

  server_key = ftok(home, PROJ_ID);
  if (server_key == (key_t)-1) {
    fprintf(stderr, "SERVER: server key is: %d\n");
    exit(1);
  }

  server_id = msgget(server_key, IPC_CREAT | QUEUE_PERMISSIONS);
  if (server_id == -1) {
    fprintf(stderr, "Wywalilo sie\n");
    exit(1);
  }
  printf("SERVER: My id is %d\n", server_id);
  // struct sigaction action;
  //
  // action.sa_handler = SIGINT_handler;
  //
  // sigemptyset(&action.sa_mask);
  // sigaddset(&action.sa_mask, SIGINT);
  // action.sa_flags = 0;
  // sigaction(SIGINT, &action, NULL);

  while(1) {
    if (msgrcv(server_id, &user_request, sizeof(struct msg), -64,0) == -1) {
      if (errno == EACCES) printf("DUPA\n");
      fprintf(stderr, "SERVER: couldn't receive msg\n");
      exit(1);
    }
    else {
      response();
    }
  }
}
