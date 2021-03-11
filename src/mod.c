#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <utils.h>
#include <mod.h>

struct Coordinate knightMaskCoords[] = {
    { 2, 1},
    { 1, 2},
    {-2, 1},
    {-1, 2},
    { 2,-1},
    { 1,-2},
    {-2,-1},
    {-1,-2},
};
struct Coordinate kingMaskCoords[] = {
    { 1, 0},
    { 1, 1},
    { 0, 1},
    {-1, 1},
    {-1, 0},
    {-1,-1},
    { 0,-1},
    { 1,-1}
};


struct Coordinate** playablePieces(struct GameState* currentState){
    unsigned int tableSize = currentState->board->width * currentState->board->height;
    bool turn = currentState->turn;
    struct Coordinate** playableCoords = calloc(tableSize+1,sizeof(struct Coordinate*));
    int width = currentState->board->width;
    int height = currentState->board->height;
    if(currentState->activePiece != NULL){
        playableCoords[ptrArrLen((void**)playableCoords)] = Coordinate_init_copy(currentState->activePiece);
    } else {
        for(int i = 0; i<width; i++){
            for(int j = 0; j<height; j++){
                enum Cell currCell = currentState->board->cells[i * height + j];
                if(turn && ((currCell == CirclesKing) || (currCell == CirclesKnight))){
                    playableCoords[ptrArrLen((void**)playableCoords)] = Coordinate_init_construct(i,j);
                } else if ((!turn) && ((currCell == CrossesKing) || (currCell == CrossesKnight))){
                    playableCoords[ptrArrLen((void**)playableCoords)] = Coordinate_init_construct(i,j);
                }
            }
        }
    }
    return playableCoords;
}

struct Move** generatePieceMoves(struct Board* board,struct Coordinate* pos,bool isActive){
    enum Cell cellType = board->cells[pos->x * board->height + pos->y];
    if(cellType == Empty || cellType == BurntGround){return calloc(1,sizeof(struct Move**));}
    
    bool isKnightLike =  cellType == CirclesKnight || cellType == CrossesKnight;
    if(isActive){
        isKnightLike = !isKnightLike;
    }
    
    struct Coordinate** pieceMask = calloc(9,sizeof(struct Coordinate*));
    if(isKnightLike){
        for(int i = 0; i<8; i++){pieceMask[ptrArrLen((void**)pieceMask)] = Coordinate_init_copy(&knightMaskCoords[i]);}
    } else {
        for(int i = 0; i<8; i++){pieceMask[ptrArrLen((void**)pieceMask)] = Coordinate_init_copy(&kingMaskCoords[i]);}
    }
    
    struct Move** output = calloc(9,sizeof(struct Move*));
    for(int i = 0; i<8; i++){output[i] = rebaseMove(pos,Move_init_construct(Coordinate_init_construct(0,0),pieceMask[i]));};

    for(int i = 0; pieceMask[i] != NULL; i++){Coordinate_destroy(pieceMask[i]);}
    free(pieceMask);

    return output;
}

struct Move** generateAllMoves(struct GameState* state){
    struct Coordinate** pPieces = playablePieces(state);
    struct Move** generatedMoves = calloc(ptrArrLen((void**)pPieces)*8 + 1,sizeof(struct Move*));
    for(int i = 0; pPieces[i] != NULL; i++){
        bool isPieceActive = state->activePiece != NULL;
        struct Move** movesForCurrPiece = generatePieceMoves(state->board,pPieces[i],isPieceActive);
        for(int j = 0; movesForCurrPiece[j] != NULL; j++){
            generatedMoves[ptrArrLen((void**)generatedMoves)] = movesForCurrPiece[j];
        }
        free(movesForCurrPiece);
    }
    return generatedMoves;
}
struct Move** filterMoves(struct Move** input,struct Board* board){
    struct Move** output = calloc(ptrArrLen((void**)input), sizeof(struct Move*));
    for(int i = 0; input[i] != NULL; i++){
        struct Coordinate* endingCoords = input[i]->to;
        if(endingCoords->x >= 0 && endingCoords->x < board->width && endingCoords->y >= 0 && endingCoords->y < board->height){
            enum Cell cellType = board->cells[endingCoords->x * board->height + endingCoords->y];
            if(cellType != BurntGround){
                output[ptrArrLen((void**)output)] = Move_init_copy(input[i]);
            }
        }
    }
    return output;
}

struct Move** possibleMoves(struct GameState* game){
    struct Move** gendMoves = generateAllMoves(game);
    struct Move** filteredMoves = filterMoves(gendMoves,game->board);
    for(int i = 0; gendMoves[i] != NULL; i++){Move_destroy(gendMoves[i]);}
    free(gendMoves);
    return filteredMoves;
}

bool isStranded(struct Board* board, struct Coordinate* pos, bool isActivePiece){
    struct Move** gendMoves = generatePieceMoves(board,pos,isActivePiece);
    struct Move** filteredMoves = filterMoves(gendMoves, board);
    for(int i = 0; gendMoves[i] != NULL; i++){Move_destroy(gendMoves[i]);}
    free(gendMoves);
    if(board->cells[pos->x * board->height + pos->y] != Empty){}
    return filteredMoves[0] == NULL;
}

struct GameState* removeStranded(struct GameState* currGame){
    struct GameState* newGame = GameState_init_copy(currGame);
    for(int i = 0; i< currGame->board->width; i++){
        for(int j = 0; j<currGame->board->height; j++){
            struct Coordinate* currPos = Coordinate_init_construct(i,j);
            int currPieceIndex = i*currGame->board->height + j;
            enum Cell currCell = currGame->board->cells[i * currGame->board->height + j];
            bool isActivePiece = false;
            if(newGame->activePiece != NULL){
                if(i == newGame->activePiece->x && j == newGame->activePiece->y){
                    isActivePiece = true;
                }
            }
            if(currGame->board->cells[currPieceIndex] != Empty){
                if(isStranded(currGame->board,currPos,isActivePiece)){
                    newGame->board->cells[i * newGame->board->height + j] = BurntGround;
                }
            }
            Coordinate_destroy(currPos);
        }
    }
    return newGame;
}

struct GameState* movePiece(struct GameState* input, struct Move* currMove){
    struct GameState* newState = GameState_init_copy(input);
    enum Cell movedPiece = newState->board->cells[currMove->from->x * input->board->height + currMove->from->y];
    newState->board->cells[currMove->from->x * input->board->height + currMove->from->y] = BurntGround;
    newState->board->cells[currMove->to  ->x * input->board->height + currMove->to  ->y] = movedPiece;
    return newState;
}

struct GameState* transition(struct GameState* currState, struct Move* currMove){
    struct Move** posMoves = possibleMoves(currState);
    bool isValid = false;
    for(int i = 0; posMoves[i] != NULL; i++){
        if( posMoves[i]->from   ->x == currMove->from->x   &&
            posMoves[i]->from   ->y == currMove->from->y   &&
            posMoves[i]->to     ->x == currMove->to->x     &&
            posMoves[i]->to     ->y == currMove->to->y
        ){
            isValid = true;
            break;
        }
    }
    if(!isValid){return NULL;};
    
    enum Cell destinationCell = currState->board->cells[currMove->to->x * currState->board->height + currMove->to->y];
    if(destinationCell == Empty){
        struct GameState* movedPiece = movePiece(currState,currMove);
        struct GameState* removedStranded = removeStranded(movedPiece);
        struct GameState* output = GameState_init_construct(
            Board_init_copy(removedStranded->board),
            !currState->turn,
            NULL
        );
        GameState_destroy(movedPiece);
        GameState_destroy(removedStranded);
        return output;
    } else {
        struct GameState* movedPiece = movePiece(currState,currMove);
        struct GameState* removedStranded = removeStranded(movedPiece);
        struct GameState* output = GameState_init_construct(
            Board_init_copy(removedStranded->board),
            currState->turn,
            Coordinate_init_construct(currMove->to->x,currMove->to->y)
        );
        GameState_destroy(movedPiece);
        GameState_destroy(removedStranded);
        return output;
    }
}
enum Cell defaultBoardCells[30] = { 1,0,0,0,3,
                                    0,0,0,0,0,
                                    2,0,0,0,4,
                                    2,0,0,0,4,
                                    0,0,0,0,0,
                                    1,0,0,0,3 };
struct Board defaultBoard = {
    .cells  = defaultBoardCells,
    .width  = 6,
    .height = 5
};