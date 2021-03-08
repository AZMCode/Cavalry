#include <stdbool.h>
#include <wchar.h>

#ifndef UTILS_H
#define UTILS_H

enum Cell{
    Empty,
    CrossesKnight,
    CrossesKing,
    CirclesKnight,
    CirclesKing,
    BurntGround
};

struct Board{
    unsigned int width;
    unsigned int height;
    enum Cell* cells;
};
struct Board* Board_init_new();
struct Board* Board_init_copy(struct Board* input);
struct Board* Board_init_construct(enum Cell* cells, unsigned int width, unsigned int height);
int Board_destroy(struct Board* board);

struct Coordinate{
    int x;
    int y;
};
struct Coordinate* Coordinate_init_new();
struct Coordinate* Coordinate_init_copy(struct Coordinate* input);
struct Coordinate* Coordinate_init_construct(int x, int y);
int Coordinate_destroy(struct Coordinate* input);

/* Representation
 * false = Friendly turn (First Player)
 * true  = Enemy turn
 */
struct GameState{
    struct Board* board;
    bool turn;
    struct Coordinate* activePiece;
};
struct GameState* GameState_init_new();
struct GameState* GameState_init_copy(struct GameState* input);
struct GameState* GameState_init_construct(struct Board* input, bool turn, struct Coordinate* activePiece);
int GameState_destroy(struct GameState* input);

struct Move{
    struct Coordinate* from;
    struct Coordinate* to;
};
struct Move* Move_init_new();
struct Move* Move_init_copy(struct Move* input);
struct Move* Move_init_construct(struct Coordinate* from, struct Coordinate* to);
int Move_destroy(struct Move* input);

struct Coordinate* numToCoord(unsigned int* input);
struct Move* coordToMove(struct Coordinate* input);
struct Coordinate* flipX(struct Coordinate* input);
struct Coordinate* flipY(struct Coordinate* input);
struct Move* rebaseMove(struct Coordinate* newBase, struct Move* input);
bool isMoveBlocked(struct Board* board,struct Move* move);
struct Coordinate** deduplicateCoords(struct Coordinate** input,int inputLen);
char* printMove(struct Move* input);
wchar_t* stringifyBoard(struct Board* board);
int ptrArrLen(void** ptrArr);
#endif