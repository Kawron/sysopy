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

#include "definitions.h"


int is_client_running = 1;

int server_id, local_id;

int command_length = 512;

struct msg client_request;
struct msg server_response;

int user_id = -1;

pid_t pid;

// void mem_cpy_command_args(struct string_array* commandArgs, int index) {
//     memcpy(client_request.msg_text.buf, commandArgs->data[index],
//            strlen(commandArgs->data[index]));
//     client_request.msg_text.buf[strlen(commandArgs->data[index])] = '\0';
// }
//
//
// void stop() {
//   client_request.msg_type = STOP;
//   client_request.msg_text.id
//
//   if (msgsnd(queue_id, &msg, MSG_SIZE, 0) == -1) {
//     fprintf(stderr, "CLIENT: couldn't send stop message\n");
//     exit(1);
//   }
//   msgctl(local_id, IPC_RMID, NULL);
// }
//
void get_list() {
  client_request.msg_type = GET_LIST;
  client_request.msg_text.sender = local_id;

  if (msgsnd(server_id, &client_request, sizeof(struct msg_text), 0) == -1) {
    fprintf(stderr, "CLIENT: sending \"GET_LIST\" request failed\n");
    exit(1);
  }
  // tego chyba nie trzeba
  // if (msgrcv(local_id, &server_response, sizeof(struct msg_text), 0, 0) == -1) {
  //   fprintf(stderr, "CLIENT: didn't receive listing response\n");
  //   exit(1);
  // }
  // printf("CLIENT: \n");
  // printf("%s\n", server_response.msg_text.buf);
}

void register_user() {
  client_request.msg_type = INIT;
  client_request.msg_text.sender = local_id;
  printf("CLIENT: server id is %d\n", server_id);
  sprintf(client_request.msg_text.buf, "%d", local_id);

  if (msgsnd(server_id, &client_request, sizeof(struct msg_text), 0) == -1) {
    fprintf(stderr, "CLIENT: Couldn't send register request\n");
    exit(1);
  }
  else {
    printf("CLIENT: Send register request\n");
  }

  if (msgrcv(local_id, &server_response, sizeof(struct msg_text), 0, 0) == -1) {
    fprintf(stderr, "CLIENT: Didn't receive register response\n");
    exit(1);
  }
  else {
    printf("CLIENT: user succesfully registered\n");
  }
}
//
void send_2all() {
  client_request.msg_type = _2ALL;
  client_request.msg_text.sender = local_id;

  printf("CLIENT: enter your message: ");
  if (fgets(client_request.msg_text.buf, sizeof(struct msg_text), stdin) == -1) {
    printf("CLIENT: something went wrong try again\n");
    return;
  }
  printf("CLIENT: Your message is%s\n", client_request.msg_text.buf);
  printf("\n");

  if (msgsnd(server_id, &client_request, sizeof(struct msg_text), 0) == -1) {
    fprintf(stderr, "CLIENT: sending msg to all failed\n");
    exit(1);
  }
}
//
// void send_2one() {
//   message msg;
//   msg.sender = local_key;
//   msg.mtype = 4;
//   msg.type = SEND_2ONE;
//
//   printf("CLIENT: enter your message: ");
//   if (fgets(msg.msg_text, MSG_SIZE, stdin) == -1) {
//     printf("CLIENT: something went wrong try again\n");
//     return;
//   }
//   printf("\n");
//
//   if (msgsnd(queue_id, &msg, MSG_SIZE, 0) == -1) {
//     fprintf(stderr, "CLIENT: sending msg to all failed\n");
//     exit(1);
//   }
// }

// void send_message() {
//     if (msgsnd(server_id, &client_request, sizeof(struct msg_text), 0) ==
//         -1) {
//         print_some_info("ERROR while sending request to the server");
//
//     } else {
//         char send_message_buffer[BUFFER_SIZE];
//
//         sprintf(send_message_buffer, "Message SENT to the server %s\n",
//                 type_to_string(client_request.msg_type));
//
//         print_some_info(send_message_buffer);
//     }
// }
//
// void stop_command(struct string_array* command_args) {
//     is_client_running = 0;
//
//     mem_cpy_command_args(command_args, 0);
// }
//
//
// void list_command(struct string_array* command_args) {
//     mem_cpy_command_args(command_args, 0);
// }
//
// void _2all_command(struct string_array* command_args) {
//     mem_cpy_command_args(command_args, 1);
// }
//
// void _2one_command(struct string_array* command_args) {
//     mem_cpy_command_args(command_args, 2);
//
//     client_request.msg_text.additional_arg =
//         strtol(command_args->data[1], NULL, 0);
// }
//
// void end_client() {
//     client_request.msg_text.id = user_id;
//     client_request.msg_type = STOP;
//
//     sprintf(client_request.msg_text.buf, "STOP from client %d", user_id);
//
//     if (msgsnd(server_id, &client_request, sizeof(struct msg_text), 0) ==
//         -1) {
//         print_error("Client: ERROR while sending data about STOP");
//     } else {
//         print_error("Client: Sending information about STOP");
//     }
//
//     if (msgctl(local_id, IPC_RMID, NULL) == -1) {
//         print_sth_and_exit("ERROR while closing client queue", 1);
//     }
//
//     kill(pid, 9);
//
//     print_some_info("Closing client!");
//
//     exit(0);
// }
//
// void handle_SIGINT(int signal_num) {
//     print_some_info("Received signal SIGINT");
//
//     end_client();
// }
//
// int execute_command(struct string_array* command_args) {
//     client_request.msg_text.id = user_id;
//
//     if (strcmp(command_args->data[0], "STOP") == 0) {
//         client_request.msg_type = STOP;
//
//         stop_command(command_args);
//
//     } else if (strcmp(command_args->data[0], "LIST") == 0) {
//         if (command_args->size != 1)
//             return 0;
//
//         client_request.msg_type = LIST;
//
//         list_command(command_args);
//     }
//
//     else if (strcmp(command_args->data[0], "2ALL") == 0) {
//         if (command_args->size != 2)
//             return 0;
//
//         client_request.msg_type = _2ALL;
//
//         _2all_command(command_args);
//
//     }
//     else if (strcmp(command_args->data[0], "2ONE") == 0) {
//         if (command_args->size != 3)
//             return 0;
//
//         client_request.msg_type = _2ONE;
//
//         _2one_command(command_args);
//
//     } else {
//         return 0;
//     }
//
//     send_message();
//
//     return 1;
// }
//
// void execute_file(struct string_array* command_args) {
//     if (command_args->size != 2) {
//         print_error("Client: invalid arguments!");
//     }
//
//     FILE* file = fopen(command_args->data[1], "r");
//
//     if (file == NULL) {
//         char error_buffer[BUFFER_SIZE];
//
//         sprintf(error_buffer, "Client: ERROR while opening file: %s \n",
//                 command_args->data[1]);
//
//         print_error(error_buffer);
//     }
//
//     long file_size = 0;
//
//     fseek(file, 0, SEEK_END);
//     file_size = ftell(file);
//     rewind(file);
//
//     char* fileContent = calloc(file_size, sizeof(char));
//
//     fread(fileContent, file_size, sizeof(char), file);
//
//     struct string_array command = process_file(fileContent, file_size, '\n');
//
//     for (int i = 0; i < command.size; i++) {
//         struct string_array command_args_inside =
//                 process_file(command.data[i], strlen(command.data[i]), ' ');
//
//         execute_command(&command_args_inside);
//
//         free(command_args_inside.data);
//     }
//
//     free(fileContent);
//     free(command.data);
// }
//
void input_loop() {
  while(1) {
    char text[256];
    printf("CLIENT: enter your message: ");
    if (fgets(text, 256, stdin) == NULL) {
      printf("CLIENT: something went wrong try again\n");
      sleep(2);
      continue;
    }
    int n = strlen(text);
    if (text[n-1] == '\n') text[n-1] = 0;
    printf("\n");
    printf("CLIENT: msg is \"%s\"\n", text);

    if (strcmp(text, "LIST") == 0) {
      get_list();
    }
    if (strcmp(text, "2ALL") == 0) {
      send_2all();
    }
    // if (strcmp(text, "2ONE") == 0) {
    //   send_2one();
    // }
    // if (strcmp(text, "STOP") == 0) {
    //   // stop();
    //   printf("CLIENT: ended client work\n");
    //   exit(0);
    // }
    sleep(1);
  }
}
//
void catcher() {

  while (1) {
    // read an incoming message, with priority order
    sleep(1);
    if (msgrcv(local_id, &server_response, sizeof(struct msg_text), -200, 0) == -1) {
      fprintf(stderr, "CLIENT: couldn't catch message from server\n");
    }
    else {
      char message_received_buffer[BUFFER_SIZE];

      sprintf(message_received_buffer,
                "Client, message received\n\t type: %ld, sender: %d, message:\n "
                "%s \n",
                server_response.msg_type, server_response.msg_text.sender,
                server_response.msg_text.buf);
      printf("%s\n", message_received_buffer);
    }
    if (server_response.msg_type == SHUTDOWN) {
        break;
    }
  }
}

int main(int argc, char* argv[]) {
    char* homedir = getenv("HOME");

    key_t server_key;

    //test
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
    //end test

    // if ((local_id = msgget(IPC_PRIVATE, 0660)) == -1) {
    //   fprintf(stderr, "CLIENT: Couldn't create local queue\n");
    //   exit(1);
    // }

    if ((server_key = ftok(homedir, PROJ_ID)) == -1) {
      fprintf(stderr, "CLIENT: Couldn't get server key\n");
      exit(1);
    }

    if ((server_id = msgget(server_key, 0)) == -1) {
      fprintf(stderr, "CLIENT: Couldn't get server queue\n");
      exit(1);
    }

    // register
    register_user();

    // main loop

    // while(1) {
    //   char text[256];
    //   printf("CLIENT: enter your message: ");
    //   if (fgets(text, 256, stdin) == NULL) {
    //     printf("CLIENT: something went wrong try again\n");
    //     continue;
    //   }
    //   int n = strlen(text);
    //   if (text[n-1] == '\n') text[n-1] = 0;
    //   printf("\n");
    //   printf("CLIENT: msg is \"%s\"\n", text);
    //
    //   if (strcmp(text, "LIST") == 0) {
    //     get_list();
    //     sleep(5);
    //   }
    //   if (strcmp(text, "2ALL") == 0) {
    //     send_2all();
    //   }
      // if (strcmp(text, "2ONE") == 0) {
      //   send_2one();
      // }
      // if (strcmp(text, "STOP") == 0) {
      //   // stop();
      //   printf("CLIENT: ended client work\n");
      //   exit(0);
      // }
    // }

    // else {
    //   char message_received_buffer[BUFFER_SIZE];
    //
    //   sprintf(message_received_buffer,
    //     "Message RECEIVED\n\ttype: %ld, id: %d, message: %s \n",
    //     server_response.msg_type, server_response.msg_text.id,
    //     server_response.msg_text.buf);
    //
    //   print_some_info(message_received_buffer);
    //
    //   user_id = server_response.msg_type;
    // }

    pid = fork();

    if (pid == 0) {
        input_loop();
    }
    else if (pid > 0) {
        // struct sigaction action;
        //
        // action.sa_handler = handle_SIGINT;
        //
        // sigemptyset(&action.sa_mask);
        // sigaddset(&action.sa_mask, SIGINT);
        //
        // action.sa_flags = 0;
        //
        // sigaction(SIGINT, &action, NULL);

        catcher();
    }
    else {
        print_error("ERROR while creating fork");
    }

    // end_client();

    return 0;
}
