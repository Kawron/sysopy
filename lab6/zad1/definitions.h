#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define MAX_CLIENTS_NUMBER 256
#define MAX_MSG_SIZE 4096
#define PROJECT_ID 0x099
#define MAX_CONT_SIZE 4096

typedef enum myType {
  SUBSCRIBE = 0,
  STOP = 1,
  GET_LIST = 2,
  SEND_2ALL = 3,
  SEND_2ONE = 4,
  INIT = 5
}myType;

typedef struct message {
  long mtype;
  myType type;
  key_t sender;
  char msg_text[MAX_CONT_SIZE];

} message;

const size_t MSG_SIZE = sizeof(message) - sizeof(long);
#endif
