#include <utils.h>

#ifndef MOD_H
#define MOD_H

struct Coordinate knightMaskCoords[8];
struct Coordinate kingMaskCoords[8];

struct Move** possibleMoves(struct GameState* currentState);
bool isStranded(struct Board* board, struct Coordinate* pos, bool isActivePiece);
struct GameState* removeStranded(struct GameState* currGame);
struct GameState* movePiece(struct GameState* input, struct Move* currMove);
struct GameState* transition(struct GameState* currState, struct Move* currMove);

enum Cell defaultBoardCells[30];
struct Board defaultBoard;

#endif