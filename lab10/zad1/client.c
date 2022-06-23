#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "common.h"

Board board;
State state = GAME_STARTING;
char* command;
char* name;
char* arg;
int server_socket;
int is_client_O;
char buffer[MAX_MESSAGE_LENGTH];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

Board new_board() {
    Board new_board = {1, {FREE}};
    return new_board;
}

object check_columns() {
    for (int x = 0; x < 3; x++) {
        object obj1 = board.objects[x];
        object obj2 = board.objects[x+3];
        object obj3 = board.objects[x+6];
        if (obj1 == obj2 && obj2 == obj3 && obj1 != FREE) {
            return obj1;
        }
    }
    return FREE;
}

object check_rows() {
    for (int x = 0; x < 3; x++) {
        object obj1 = board.objects[3*x];
        object obj2 = board.objects[3*x + 1];
        object obj3 = board.objects[3*x + 2];
        if (obj1 == obj2 && obj2 == obj3 && obj1 != FREE) {
            return obj1;
        }
    }
    return FREE;
}

object check_diagonals() {
    object obj1, obj2, obj3;
    obj1 = board.objects[0];
    obj2 = board.objects[4];
    obj3 = board.objects[8];
    if (obj1 == obj2 && obj2 == obj3 && obj1 != FREE) {
        return obj1;
    }
    obj1 = board.objects[2];
    obj2 = board.objects[4];
    obj3 = board.objects[6];
    if (obj1 == obj2 && obj2 == obj3 && obj1 != FREE) {
        return obj1;
    }

    return FREE;
}

void quit() {
    sprintf(buffer, "quit: :%s", name);
    send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
    exit(0);
}

int move(int position) {
    if (position < 0 || position > 8 || board.objects[position] != FREE) {
        return 0;
    }
    if (board.move == 1) {
        board.objects[position] = O;
    }
    else {
        board.objects[position] = X;
    }
    board.move = !board.move;
    return 1;
}

void draw() {
    // system("clear");
    char symbol;
    int idx;
    for (int y = 0; y < 3; y++) {
        printf("\n");
        for (int x = 0; x < 3; x++) {
            idx = y*3 + x;
            if (board.objects[idx] == FREE) {
                symbol = idx + 49;
            }
            else if (board.objects[idx] == O) {
                symbol = 'O';
            }
            else {
                symbol = 'X';
            }
            printf("  %c  ", symbol);
        }
        printf("\n");
        printf("_______________");
        printf("\n");
    }
}

int is_board_empty() {
    int flag = 1;
    for (int i = 0; i < 9; i++) {
        if (board.objects[i] == FREE) {
            flag = 0;
            break;
        }
    }
    return flag;
}

void check_game() {
    object winner;
    winner = check_rows();

    if (winner == FREE) {
        winner = check_columns();
    }
    if (winner == FREE) {
        winner = check_diagonals();
    }
    if (winner != FREE) {
        if ((is_client_O && winner == O) || (!is_client_O && winner == X)) {
            printf("YOU WON!\n");
        }
        else {
            printf("YOU LOST!\n");
        }
        state = QUIT;
    }
    if (is_board_empty()) {
        printf("DRAW!\n");
        state = QUIT;
    }
}

void play_game() {

    while (1) {

        if (state == GAME_STARTING) {
            if (strcmp(arg, "name_taken") == 0) {
                fprintf(stderr, "this name is already taken\n");
                exit(1);
            }
            else if (strcmp(arg, "no_enemy") == 0) {
                printf("Waiting for opponent.\n");
                state = WAITING;
            }
            else {
                board = new_board();
                if (arg[0] == 'O') {
                    is_client_O = 1;
                    state = MOVE;
                }
                else {
                    is_client_O = 0;
                    state = WAITING_FOR_MOVE;
                }
            }
        }
        else if (state == WAITING) {
            pthread_mutex_lock(&mutex);
            while (state != GAME_STARTING && state != QUIT) {
                pthread_cond_wait(&cond, &mutex);
            }
            pthread_mutex_unlock(&mutex);
            board = new_board();
            if (arg[0] == 'O') {
                is_client_O = 1;
                state = MOVE;
            }
            else {
                is_client_O = 0;
                state = WAITING_FOR_MOVE;
            }
        }
        else if (state == WAITING_FOR_MOVE) {
            printf("Waiting for opponent's move.\n");
            pthread_mutex_lock(&mutex);
            while (state != OPPONENT_MOVE && state != QUIT) {
                pthread_cond_wait(&cond, &mutex);
            }
            pthread_mutex_unlock(&mutex);
        }
        else if (state == OPPONENT_MOVE) {
            int pos = atoi(arg);
            move(pos);
            check_game();
            if (state != QUIT) {
                state = MOVE;
            }
        }
        else if (state == MOVE) {
            draw();
            int pos = -1;
            while(!move(pos)) {
                if (is_client_O) {
                    printf("ENTER YOUR MOVE (O): ");
                }
                else {
                    printf("ENTER YOUR MOVE (X): ");
                }
                scanf("%d", &pos);
                pos--;
            }
            draw();
            sprintf(buffer, "move:%d:%s", pos, name);
            send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
            check_game();
            if (state != QUIT) {
                state = WAITING_FOR_MOVE;
            }
        }
        else if (state == QUIT) {
            quit();
        }
    }
}

void init_local_socket(char* path) {
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un local_sockaddr;

    memset(&local_sockaddr, 0, sizeof(struct sockaddr_un));
    local_sockaddr.sun_family = AF_UNIX;
    strcpy(local_sockaddr.sun_path, path);
    connect(server_socket, (struct sockaddr*) &local_sockaddr, sizeof(struct sockaddr_un));
}

void init_network_socket(char* service) {
    struct addrinfo hints;
    struct addrinfo* res;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo("localhost", service, &hints, &res);
    server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    connect(server_socket, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
}

void listen_server() {

    int game_thread_running = 0;
    while (1) {

        recv(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);

        command = strtok(buffer, ":");
        arg = strtok(NULL, ":");
        pthread_mutex_lock(&mutex);

        if (strcmp(command, "add") == 0) {
            state = GAME_STARTING;
            if (!game_thread_running) {
                game_thread_running = !game_thread_running;
                pthread_t t;
                pthread_create(&t, NULL, (void *(*)(void *)) play_game, NULL);
            }
        }
        else if (strcmp(command, "move") == 0) {
            state = OPPONENT_MOVE;
        }
        else if (strcmp(command, "quit") == 0) {
            state = QUIT;
            exit(0);
        }
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char* argv[]) {

    if (argc != 4) {
        fprintf(stderr, "WRONG NUM OF ARGUMENTS\n");
        exit(1);
    }

    char* type = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
    char* destination = calloc(MAX_MESSAGE_LENGTH, sizeof(char));

    name = argv[1];
    type = argv[2];
    destination = argv[3];

    signal(SIGINT, quit);

    if (strcmp(type, "local") == 0) {
        init_local_socket(destination);
    }
    else {
        init_network_socket(destination);
    }

    sprintf(buffer, "add: :%s", name);
    send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
    listen_server();
}
