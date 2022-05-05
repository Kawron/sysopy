#include <libgen.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <mqueue.h>

#include "definitions.h"

int clients_keys[MAX_CLIENTS_NUMBER] = {-1};
key_t server_key;
mqd_t server_id;
int piority;
char message[1064];
char user_path[1064];

void register_user() {
  int user_key = atoi(message);
  int flag = 0;
  for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
    if (clients_keys[i] == -1) {
      clients_keys[i] = user_key;
      flag = 1;
      break;
    }
  }
  if (flag == 0) {
    printf("SERVER: not enough place for more users\n");
    return;
  }

  printf("SERVER: user with key %d was just registered\n", user_key);
  sprintf(user_path, "/%d", user_key);
  strcpy(message, "SERVER: success");

  mqd_t user_queue_id = mq_open(user_path, O_WRONLY);
  mq_send(user_queue_id, &message, MESSAGE_SIZE, INIT);
}

void listing_request() {
  int user_key = atoi(message);

  char buffor[1064];
  int idx = 0;
  for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
    if (clients_keys[i] != -1) {
      idx += sprintf(&buffor[idx], "%d\n", clients_keys[i]);
    }
  }
  buffor[idx] = 0;

  strcpy(message, buffor);
  printf("SERVER: user with key %d requested listing\n", user_key);
  sprintf(user_path, "/%d", user_key);

  mqd_t user_queue_id = mq_open(user_path, O_WRONLY);
  mq_send(user_queue_id, &message, MESSAGE_SIZE, INIT);
}

void send_2all() {

  printf("SERVER: sending message to everybody\n");

  for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
    if (clients_keys[i] != 1) {
      sprintf(user_path, "/%d", clients_keys[i]);
      mqd_t user_queue_id = mq_open(user_path, O_WRONLY);
      mq_send(user_queue_id, &message, MESSAGE_SIZE, SEND_2ALL);
    }
  }
}

void user_send_2one() {
  int user_key = atoi(message);

  mq_receive(server_id, &message, sizeof(char)*1064, &piority);

  sprintf(user_path, "/%d", user_key);
  mqd_t user_queue_id = mq_open(user_path, O_WRONLY);
  mq_send(user_queue_id, &message, MESSAGE_SIZE, SEND_2ALL);
}

void user_stoped() {
  printf("SERVER: user %s is closed\n", message);

  for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
    if (clients_keys[i] == atoi(message)) {
      clients_keys[i] = -1;
      return;
    }
  }
  fprintf(stderr, "SERVER: couldn't delete user\n");
}

void close_clients() {
  sprintf(message, "SERVER: time to close");

  for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
    if (clients_keys[i] != -1) {
      sprintf(user_path, "/%d", clients_keys[i]);
      mqd_t user_queue_id = mq_open(user_path, O_WRONLY);
      mq_send(user_queue_id, &message, MESSAGE_SIZE, CLOSING_SERVER);
    }
  }
}
void close_server() {
  if (mq_close(server_id) == -1) {
    fprintf(stderr, "SERVER: couldn't close queue\n");
    exit(1);
  }
  if (mq_unlink(server_path) == -1) {
    fprintf(stderr, "SERVER: couldn't destroy queue\n");
    exit(1);
  }
}

void SIGINT_handler(int signal_num) {
  close_clients();
  sleep(2);
  close_server();
}

void response() {
  switch (piority) {
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

int main(int argc, char** argv) {
  for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
    clients_keys[i] = -1;
  }

  struct mq_attr server_attr;
  server_attr.mq_maxmsg = MAX_MESSAGE_QUEUE_SIZE;
  server_attr.mq_msgsize = MESSAGE_SIZE;

  server_id = mq_open(server_path, O_RDWR | O_CREAT | O_EXCL, QUEUE_PERMISSIONS, &server_attr);
  printf("SERVER: My path is %d\n", server_path);

  struct sigaction action;

  action.sa_handler = SIGINT_handler;
  action.sa_flags = 0;

  sigaction(SIGINT, &action, NULL);

  while(1) {
    if (mq_receive(server_id, &message, MESSAGE_SIZE, &piority) == -1) {
      fprintf(stderr, "SERVER: couldn't receive msg\n");
      exit(1);
    }
    else {
      response();
    }
  }
}
