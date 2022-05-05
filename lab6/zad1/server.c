#include <fcntl.h>
#include <stdlib.h>
#include <libgen.h>
#include <stdio.h>
#include <memory.h>
#include <ctype.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <time.h>
#include <signal.h>

#include "definitions.h"

key_t clients_id[MAX_CLIENTS_NUMBER] = {-1};
key_t server_key;
int server_id = 0;
struct msg user_request;
struct msg server_response;

void log(int user_id, char* message){
  FILE *f = fopen("log.txt","a");
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  fprintf(f,"%d-%02d-%02d %02d:%02d:%02d>> user_id: %d,  message: %s\n",
   tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, user_id, message);
  fclose(f);
}

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
  strcpy(server_response.msg_text.buffor, "#log: user registered");

  printf("SERVER: user with id %d was just registered\n", user_id);
  msgsnd(user_id, &server_response, sizeof(struct msg_text), 0);
  log(user_id, server_response.msg_text.buffor);
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
  strcpy(server_response.msg_text.buffor, buffor);
  server_response.msg_text.sender = server_id;

  printf("SERVER: user with id %d requested listing\n", user_id);
  msgsnd(user_id, &server_response, sizeof(struct msg_text), 0);
  log(user_id, server_response.msg_text.buffor);
}

void send_2all() {
  int user_id = user_request.msg_text.sender;

  server_response.msg_type = SEND_2ALL;
  server_response.msg_text.sender = user_id;
  strcpy(server_response.msg_text.buffor, user_request.msg_text.buffor);

  printf("SERVER: user with id %d send message to all users\n", user_id);

  for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
    if (clients_id[i] != 1) {
      msgsnd(clients_id[i], &server_response, sizeof(struct msg_text), 0);
      log(clients_id[i], server_response.msg_text.buffor);
    }
  }
}

void user_send_2one() {
  int user_id = user_request.msg_text.sender;

  server_response.msg_type = SEND_2ALL;
  server_response.msg_text.sender = user_id;
  strcpy(server_response.msg_text.buffor, user_request.msg_text.buffor);

  printf("SERVER: user with id %d send message to user %d\n", user_id, user_request.msg_text.address);

  msgsnd(user_request.msg_text.address, &server_response, sizeof(struct msg_text), 0);
  log(user_request.msg_text.address, server_response.msg_text.buffor);

}
void user_stoped() {
  int user_key = user_request.msg_text.sender;
  printf("SERVER: user %d is closed\n", user_key);

  for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
    if (clients_id[i] == user_key) {
      clients_id[i] = -1;
      return;
    }
  }
  fprintf(stderr, "SERVER: couldn't delete user\n");
}

void response() {
  switch (user_request.msg_type) {
    case INIT:
      register_user();
      break;
    case STOP:
      user_stoped();
      break;
    case GET_LIST:
      listing_request();
      break;
    case SEND_2ALL:
      send_2all();
      break;
    case SEND_2ONE:
      user_send_2one();
      break;
    default:
      break;
  }
}

void close_clients() {
  server_response.msg_type = CLOSING_SERVER;
  server_response.msg_text.sender = server_id;

  for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
    if (clients_id[i] != -1) {
      msgsnd(clients_id[i], &server_response, sizeof(struct msg_text), 0);
      log(clients_id[i], server_response.msg_text.buffor);
    }
  }
}
void close_server() {
  if (msgctl(server_id, IPC_RMID, NULL) == -1) {
    fprintf(stderr, "SERVER: couldn't close queue\n");
    exit(1);
  }
  exit(0);
}

void SIGINT_handler(int signal_num) {
  close_clients();
  sleep(2);
  close_server();
}

int main(int argc, char** argv) {
  char* home = getenv("HOME");

  for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
      clients_id[i] = -1;
  }

  server_key = ftok(home, PROJECT_ID);
  if (server_key == (key_t)-1) {
    fprintf(stderr, "SERVER: server key is: %d\n");
    exit(1);
  }

  server_id = msgget(server_key, IPC_CREAT | QUEUE_PERMISSIONS);
  if (server_id == -1) {
    fprintf(stderr, "SERVER: server couldn't create queue");
    exit(1);
  }
  printf("SERVER: My id is %d\n", server_id);

  struct sigaction action;

  action.sa_handler = SIGINT_handler;
  action.sa_flags = 0;

  sigaction(SIGINT, &action, NULL);

  while(1) {
    if (msgrcv(server_id, &user_request, sizeof(struct msg), -10,0) == -1) {
      fprintf(stderr, "SERVER: couldn't receive msg\n");
      exit(1);
    }
    else {
      response();
    }
  }
}
