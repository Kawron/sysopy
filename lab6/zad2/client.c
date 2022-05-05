#include <libgen.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <memory.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mqueue.h>

#include "definitions.h"

mqd_t server_id;
mqd_t local_id;
int local_key;

int piority;
pid_t pid;
char local_queue_name[1064];
char message[1064];

void get_list() {
  sprintf(message, "%d", local_key);
  if (mq_send(server_id, &message, MESSAGE_SIZE, GET_LIST) == -1) {
    fprintf(stderr, "CLIENT: sending listing request failed\n");
    exit(1);
  }
}

void register_user() {
  sprintf(message, "%d", local_key);

  if (mq_send(server_id, &message, MESSAGE_SIZE, INIT) == -1) {
    fprintf(stderr, "CLIENT: sending login request failed\n");
    exit(1);
  }
  sleep(2);
  if (mq_receive(local_id, &message, MESSAGE_SIZE, piority) == -1) {
    fprintf(stderr, "CLIENT: couldn't get server response to login request\n");
    exit(1);
  }
  printf("%s\n", message);
}

void send_2all() {
  char message[1064];

  printf("CLIENT: enter your message: ");
  if (fgets(message, MESSAGE_SIZE, stdin) == -1) {
    printf("CLIENT: something went wrong try again\n");
    return;
  }

  if (mq_send(server_id, &message, MESSAGE_SIZE, SEND_2ALL) == -1) {
    fprintf(stderr, "CLIENT: sends message to all failed\n");
    exit(1);
  }
}

void send_2one() {
  char message[1064];

  printf("CLIENT: enter address: ");
  if (fgets(message, MESSAGE_SIZE, stdin) == -1) {
    printf("CLIENT: something went wrong try again\n");
    return;
  }

  if (mq_send(server_id, &message, MESSAGE_SIZE, SEND_2ONE) == -1) {
    fprintf(stderr, "CLIENT: sends address to server\n");
    exit(1);
  }

  printf("CLIENT: enter message: ");
  if (fgets(message, MESSAGE_SIZE, stdin) == -1) {
    printf("CLIENT: something went wrong try again\n");
    return;
  }

  if (mq_send(server_id, &message, MESSAGE_SIZE, SEND_2ONE) == -1) {
    fprintf(stderr, "CLIENT: sends message to another user\n");
    exit(1);
  }
}

void stop() {
  sprintf(message, "%d", local_key);

  if (mq_send(server_id, &message, MESSAGE_SIZE, STOP) == -1) {
    fprintf(stderr, "CLIENT: sending stop info failed\n");
    exit(1);
  }

  if (mq_close(local_id) == -1) {
    fprintf(stderr, "CLIENT: couldn't close queue\n");
    exit(1);
  }
  if (mq_unlink(local_queue_name) == -1) {
    fprintf(stderr, "CLIENT: couldn't destroy queue\n");
    exit(1);
  }
  printf("CLIENT is closing\n");
  kill(pid, SIGTERM);
  exit(0);
}

void handle_SIGINT(int signal_num) {
    printf("\nCLIENT: received sigint");
    stop();
}

void input_loop() {
  while(1) {
    char text[256];
    printf("CLIENT: what do you want to do: ");
    if (fgets(text, 256, stdin) == NULL) {
      printf("CLIENT: something went wrong try again\n");
      sleep(2);
      continue;
    }
    int n = strlen(text);
    if (text[n-1] == '\n') text[n-1] = 0;

    if (strcmp(text, "LIST") == 0) {
      get_list();
    }
    else if (strcmp(text, "2ALL") == 0) {
      send_2all();
    }
    else if (strcmp(text, "2ONE") == 0) {
      send_2one();
    }
    else if (strcmp(text, "STOP") == 0) {
      stop();
    }
    else {
      printf("CLIENT: unknown command, try again\n");
    }
    sleep(1);
  }
}

void catcher() {
  while (1) {
    sleep(2);
    if (mq_receive(local_id, &message, MESSAGE_SIZE, &piority) == -1) {
      fprintf(stderr, "CLIENT: couldn't catch message from server\n");
    }
    else {
      if (piority == CLOSING_SERVER) {
        printf("\nCLIENT: server is closing\n");
        stop();
      }
      char message_from_server[1064];
      sprintf(message_from_server,
                "\nCLIENT: message received\n type: %d\n message:\n%s\n",
                piority, message);
      printf("%s\n", message_from_server);
      printf("\nCLIENT: what do you want to do: ");
    }
  }
}

int main(int argc, char* argv[]) {
    char* homedir = getenv("HOME");

    server_id = mq_open(server_path, O_WRONLY);
    if (server_id == -1) {
      fprintf(stderr, "CLIENT: Couldn't get server queue\n");
      exit(1);
    }

    struct mq_attr myQueueAttr;
    myQueueAttr.mq_maxmsg = MAX_MESSAGE_QUEUE_SIZE;
    myQueueAttr.mq_msgsize = MESSAGE_SIZE;

    local_key = ftok(homedir, getpid());
    if (local_key == -1) {
      fprintf(stderr, "CLIENT: couldn't create local_key\n");
      exit(1);
    }
    sprintf(local_queue_name, "/%d", local_key);
    printf("CLIENT: queue name %s\n", local_queue_name);

    local_id = mq_open(local_queue_name, O_RDWR | O_CREAT | O_EXCL, 0700, &myQueueAttr);
    if (local_id == -1) {
      fprintf(stderr, "CLIENT: couldn't create local queue\n");
      exit(1);
    }

    register_user();

    pid = fork();

    if (pid == 0) {
      struct sigaction action;

      action.sa_handler = handle_SIGINT;
      action.sa_flags = 0;

      sigaction(SIGINT, &action, NULL);

      input_loop();
    }
    else if (pid > 0) {
      catcher();
    }
    else {
      fprintf(stderr, "CLIENT: Couldn't fork\n");
      exit(1);
    }

    return 0;
}
