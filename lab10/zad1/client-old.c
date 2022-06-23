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
// to możńa przeniesić wszczedzie gdzie trzeba
char* command;
char* name;
char* arg;
int server_socket;
int is_client_O;
char buffer[MAX_MESSAGE_LENGTH];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t mutex_cond = PTHREAD_COND_INITIALIZER;

Board create_board() {
    Board board = {1, {FREE}};
    return board;
}

mark check_winner() {
    mark column = FREE;
    for (int x = 0; x < 3; x++) {
        mark first = board.marks[x];
        mark second = board.marks[x + 3];
        mark third = board.marks[x + 6];
        if (first == second && first == third && first != FREE) {
            column = first;
        }
    }
    if (column != FREE) {
        return column;
    }
    mark row = FREE;
    for (int y = 0; y < 3; y++) {
        mark first = board.marks[3 * y];
        mark second = board.marks[3 * y + 1];
        mark third = board.marks[3 * y + 2];
        if (first == second && first == third && first != FREE) {
            row = first;
        }
    }
    if (row != FREE) {
        return row;
    }
    mark lower_diagonal = FREE;
    mark first = board.marks[0];
    mark second = board.marks[4];
    mark third = board.marks[8];
    if (first == second && first == third && first != FREE) {
        lower_diagonal = first;
    }
    if (lower_diagonal != FREE) {
        return lower_diagonal;
    }
    mark upper_diagonal = FREE;
    first = board.marks[2];
    second = board.marks[4];
    third = board.marks[6];
    if (first == second && first == third && first != FREE) {
        upper_diagonal = first;
    }
    return upper_diagonal;
}

void quit() {
    // usunalem temp_buffer i dodałem buffer
    sprintf(buffer, "quit: :%s", name);
    send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
    exit(0);
}

int move(int position) {
    if (position < 0 || position > 9 || board.marks[position] != FREE)
        return 0;
    board.marks[position] = board.move ? O : X;
    board.move = !board.move;
    return 1;
}

void draw() {
    system("clear");
    char symbol;
    int idx;
    for (int y = 0; y < 3; y++) {
        printf("\n");
        for (int x = 0; x < 3; x++) {
            idx = y*3 + x;
            if (board.marks[idx] == FREE) {
                symbol = idx + 49;
            }
            else if (board.marks[idx] == O) {
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

void check_game() {
    int win = 0;
    mark winner = check_winner();
    if (winner != FREE) {
        if ((is_client_O && winner == O) || (!is_client_O && winner == X))
            printf("WIN!\n");
        else
            printf("LOST!\n");
        win = 1;
    }
    int draw = 1;
    for (int i = 0; i < 9; i++) {
        if (board.marks[i] == FREE) {
            draw = 0;
            break;
        }
    }
    if (draw && !win)
        printf("DRAW!\n");
    if (win || draw)
        state = QUIT;
}

void play_game() {
    while (1) {
        if (state == GAME_STARTING) {
            if (strcmp(arg, "name_taken") == 0) {
                perror("Name is already taken.\n");
                exit(1);
            }
            else if (strcmp(arg, "no_enemy") == 0) {
                printf("Waiting for opponent.\n");
                state = WAITING;
            }
            else {
                board = create_board();
                is_client_O = arg[0] == 'O';
                state = is_client_O ? MOVE : WAITING_FOR_MOVE;
            }
        }
        else if (state == WAITING) {
            pthread_mutex_lock(&mutex);
            while (state != GAME_STARTING && state != QUIT)
                pthread_cond_wait(&mutex_cond, &mutex);
            pthread_mutex_unlock(&mutex);
            board = create_board();
            is_client_O = arg[0] == 'O';
            state = is_client_O ? MOVE : WAITING_FOR_MOVE;
        }
        else if (state == WAITING_FOR_MOVE) {
            printf("Waiting for opponent's move.\n");
            pthread_mutex_lock(&mutex);
            while (state != OPPONENT_MOVE && state != QUIT)
                pthread_cond_wait(&mutex_cond, &mutex);
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
            int pos;
            do {
                printf("Next move (%c): ", is_client_O ? 'O' : 'X');
                scanf("%d", &pos);
                pos--;
            } while (!move(pos));
            draw();
            char temp_buffer[MAX_MESSAGE_LENGTH];
            sprintf(temp_buffer, "move:%d:%s", pos, name);
            send(server_socket, temp_buffer, MAX_MESSAGE_LENGTH, 0);
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

void init_server_connection(char* type, char* destination) {
    if (strcmp(type, "local") == 0) {
        server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
        struct sockaddr_un local_sockaddr;
        memset(&local_sockaddr, 0, sizeof(struct sockaddr_un));
        local_sockaddr.sun_family = AF_UNIX;
        strcpy(local_sockaddr.sun_path, destination);
        connect(server_socket, (struct sockaddr* ) &local_sockaddr, sizeof(struct sockaddr_un));
    } else {
        struct addrinfo* info;
        struct addrinfo hints;
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        getaddrinfo("localhost", destination, &hints, &info);
        server_socket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
        connect(server_socket, info->ai_addr, info->ai_addrlen);
        freeaddrinfo(info);
    }
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
                pthread_t t;
                pthread_create(&t, NULL, (void *(*)(void *)) play_game, NULL);
                game_thread_running = 1;
            }
        }
        else if (strcmp(command, "move") == 0) {
            state = OPPONENT_MOVE;
        }
        else if (strcmp(command, "quit") == 0) {
            state = QUIT;
            exit(0);
        }
        else if (strcmp(command, "ping") == 0) {
            sprintf(buffer, "pong: :%s", name);
            send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
        }
        pthread_cond_signal(&mutex_cond);
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char* argv[]) {

    if (argc != 4) {
        fprintf(stderr, "WRONG NUM OF ARGUMENTS\n");
        exit(1);
    }

    char* type = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
    char* service = calloc(MAX_MESSAGE_LENGTH, sizeof(char));

    name = argv[1];
    type = argv[2];
    service = argv[3];

    signal(SIGINT, quit);

    // if (strcmp(type, "local") == 0) {
    //     init_local_socket(service);
    // }
    // else {
    //     init_network_socket(service);
    // }

    init_server_connection(type, service);

    sprintf(buffer, "add: :%s", name);
    send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
    listen_server();
}
