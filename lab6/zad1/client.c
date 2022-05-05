#include <libgen.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <memory.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>

#include "definitions.h"

int server_id, local_id;

struct msg client_request;
struct msg server_response;

pid_t pid;

void get_list() {
  client_request.msg_type = GET_LIST;
  client_request.msg_text.sender = local_id;

  if (msgsnd(server_id, &client_request, sizeof(struct msg_text), 0) == -1) {
    fprintf(stderr, "CLIENT: sending \"GET_LIST\" request failed\n");
    exit(1);
  }
}

void register_user() {
  client_request.msg_type = INIT;
  client_request.msg_text.sender = local_id;
  printf("CLIENT: server id is %d\n", server_id);
  sprintf(client_request.msg_text.buffor, "%d", local_id);

  if (msgsnd(server_id, &client_request, sizeof(struct msg_text), 0) == -1) {
    fprintf(stderr, "CLIENT: couldn't send register request\n");
    exit(1);
  }
  else {
    printf("CLIENT: send register request\n");
  }

  if (msgrcv(local_id, &server_response, sizeof(struct msg_text), 0, 0) == -1) {
    fprintf(stderr, "CLIENT: didn't receive register response\n");
    exit(1);
  }
  else {
    printf("CLIENT: user succesfully registered\n");
  }
}

void send_2all() {
  client_request.msg_type = SEND_2ALL;
  client_request.msg_text.sender = local_id;

  printf("CLIENT: enter your message: ");
  if (fgets(client_request.msg_text.buffor, sizeof(struct msg_text), stdin) == -1) {
    printf("CLIENT: something went wrong try again\n");
    return;
  }
  printf("CLIENT: Your message is %s\n", client_request.msg_text.buffor);
  printf("\n");

  if (msgsnd(server_id, &client_request, sizeof(struct msg_text), 0) == -1) {
    fprintf(stderr, "CLIENT: sending msg to all failed\n");
    exit(1);
  }
}

void send_2one() {
  client_request.msg_type = SEND_2ONE;
  client_request.msg_text.sender = local_id;

  char bufforfor[64];

  printf("CLIENT: to who you want address your message: ");
  if (fgets(bufforfor, sizeof(char)*64, stdin) == -1) {
    printf("CLIENT: something went wrong try again\n");
    return;
  }
  client_request.msg_text.address = atoi(bufforfor);
  printf("CLIENT: enter your message: ");
  if (fgets(client_request.msg_text.buffor, sizeof(struct msg_text), stdin) == -1) {
    printf("CLIENT: something went wrong try again\n");
    return;
  }
  printf("CLIENT: Your message is %s\n", client_request.msg_text.buffor);
  printf("\n");

  if (msgsnd(server_id, &client_request, sizeof(struct msg_text), 0) == -1) {
    fprintf(stderr, "CLIENT: sending msg to all failed\n");
    exit(1);
  }
}

void stop() {
    client_request.msg_text.sender = local_id;
    client_request.msg_type = STOP;

    if (msgsnd(server_id, &client_request, sizeof(struct msg_text), 0) ==-1) {
      fprintf(stderr, "CLIENT: couldn't send stop msg to server\n");
      exit(1);
    }

    if (msgctl(local_id, IPC_RMID, NULL) == -1) {
      fprintf(stderr, "CLIENT: couldn't close queue\n");
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
    if (msgrcv(local_id, &server_response, sizeof(struct msg_text), -10, 0) == -1) {
      fprintf(stderr, "CLIENT: couldn't catch message from server\n");
    }
    else {
      if (server_response.msg_type == CLOSING_SERVER) {
        printf("\nCLIENT: server is closing\n");
        stop();
      }
      char message_from_server[MESSAGE_MAX_SIZE];

      sprintf(message_from_server,
                "\nCLIENT: message received\n type: %ld\n sender: %d\n message:\n%s\n",
                server_response.msg_type, server_response.msg_text.sender,
                server_response.msg_text.buffor);
      printf("%s\n", message_from_server);
      printf("\nCLIENT: what do you want to do: ");
    }
  }
}

int main(int argc, char* argv[]) {
    char* homedir = getenv("HOME");

    key_t server_key;

    int local_key = ftok(homedir, getpid());
    if (local_key == -1) {
      fprintf(stderr, "Wywalilo sie\n");
      exit(1);
    }

    local_id = msgget(local_key, IPC_CREAT | IPC_EXCL | 0666);
    if (local_id == -1) {
      fprintf(stderr, "Wywalilo sie\n");
      exit(1);
    }
    printf("CLIENT: my id is %d\n", local_id);

    if ((server_key = ftok(homedir, PROJECT_ID)) == -1) {
      fprintf(stderr, "CLIENT: Couldn't get server key\n");
      exit(1);
    }

    if ((server_id = msgget(server_key, 0)) == -1) {
      fprintf(stderr, "CLIENT: Couldn't get server queue\n");
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
