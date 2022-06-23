#define _POSIX_C_SOURCE 200112L

#define MAX_PLAYERS 20
#define MAX_BACKLOG 10
#define MAX_MESSAGE_LENGTH 256
#define _POSIX_C_SOURCE 200112L

typedef struct {
    char* name;
    int fd;
} client;

typedef enum {
    FREE,
    O,
    X
} object;

typedef struct {
    int move;
    object objects[9];
} Board;

typedef enum {
    GAME_STARTING,
    WAITING,
    WAITING_FOR_MOVE,
    OPPONENT_MOVE,
    MOVE,
    QUIT
} State;
