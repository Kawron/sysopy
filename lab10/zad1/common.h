#define MAX_PLAYERS 20
#define MAX_BACKLOG 10
#define MAX_MESSAGE_LENGTH 256
#define _POSIX_C_SOURCE 200112L

typedef struct {
    char* name;
    int fd;
    int online;
} client;

typedef enum {
    FREE,
    O,
    X
} mark;

typedef struct {
    int move;
    mark marks[9];
} Board;

typedef enum {
    START,
    MATCHMAKING,
    WAITING_FOR_MOVE,
    OPPONENT_MOVE,
    MOVE,
    EXIT
} Game_mode;
