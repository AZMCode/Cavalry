#include <utils.h>

#ifndef MOD_H
#define MOD_H

struct Coordinate knightMaskCoords[8];
struct Coordinate kingMaskCoords[8];

struct Coordinate** playablePieces(struct GameState* currentState);
struct Move** generatePieceMoves(struct Board* board,struct Coordinate* pos,bool isActive);
struct Move** generateAllMoves(struct GameState* state);
struct Move** filterMoves(struct Move** input,struct Board* board);
struct Move** possibleMoves(struct GameState* game);
bool isStranded(struct Board* board, struct Coordinate* pos, bool isActivePiece);
struct GameState* removeStranded(struct GameState* currGame);
struct GameState* movePiece(struct GameState* input, struct Move* currMove);
struct GameState* transition(struct GameState* currState, struct Move* currMove);

enum Cell defaultBoardCells[30];
struct Board defaultBoard;

#endif