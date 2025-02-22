#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define PROJECT_ID 123
#define MAX_CLIENTS_NUMBER 20
#define MESSAGE_MAX_SIZE 1024
#define QUEUE_PERMISSIONS 0660
#define STOP 1
#define GET_LIST 2
#define INIT 3
#define SEND_2ALL 4
#define SEND_2ONE 5
#define CLOSING_SERVER 6


struct msg_text {
    int sender;
    int address;
    char buffor[256];
};

struct msg {
    long msg_type;
    struct msg_text msg_text;
};

#endif  // DEFINITIONS_H
