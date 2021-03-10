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


struct Move** possibleMoves(struct GameState* currentState){
    unsigned int tableSize = currentState->board->width * currentState->board->height;
    bool turn = currentState->turn;
    struct Coordinate** playableCoords = calloc(tableSize+1,sizeof(struct Coordinate*));
    
    if(currentState->activePiece != NULL){
        playableCoords[ptrArrLen((void**)(playableCoords))] = Coordinate_init_copy(currentState->activePiece);
    } else {
        for(int i = 0; i<currentState->board->width; i++){
            for(int j = 0; j<currentState->board->height;j++){
                enum Cell currCellType = currentState->board->cells[i*currentState->board->height + j];
                int nextIndex = ptrArrLen((void**)(playableCoords));
                if((!turn) && (currCellType == 1 || currCellType == 2)){
                    playableCoords[nextIndex] = Coordinate_init_construct(i,j);
                } else if(turn && (currCellType == 3 || currCellType == 4)){
                    playableCoords[nextIndex] = Coordinate_init_construct(i,j);
                }
            }
        }
            
    }
    struct Move** legalMoves = calloc(9*ptrArrLen((void**)playableCoords),sizeof(struct Move*));
    for(int i = 0; playableCoords[i] != NULL; i++){
        struct Coordinate* coord= Coordinate_init_copy(playableCoords[i]);
        enum Cell cellType = currentState->board->cells[coord->x * currentState->board->height + coord->y];
        bool knightLike =  (cellType%2 == 1) != (currentState->activePiece != NULL);
        struct Coordinate* currMask = knightLike ? knightMaskCoords : kingMaskCoords;
        struct Coordinate** rebasedMask = calloc(9,sizeof(struct Coordinate*));
        struct Coordinate** legalMask = calloc(9,sizeof(struct Coordinate*));
        for(int j = 0; j < 8; j++){
            rebasedMask[j] = Coordinate_init_construct(currMask[j].x + coord->x,currMask[j].y + coord->y);
            bool isInBoardX = rebasedMask[j]->x > 0 && rebasedMask[j]->x < currentState->board->width;
            bool isInBoardY = rebasedMask[j]->y > 0 && rebasedMask[j]->y < currentState->board->height;
            if(isInBoardX && isInBoardY){
                if(currentState->board->cells[(rebasedMask[j]->x * currentState->board->height)+(rebasedMask[j]->y)] != BurntGround){
                    legalMask[ptrArrLen((void**)(legalMask))] = Coordinate_init_copy(rebasedMask[j]);
                }
            }
        }
        for(int j = 0; j< ptrArrLen((void**)(legalMask));j++){
            legalMoves[j] = Move_init_construct(coord,legalMask[j]);
        }
        
        //Destroying the Coordinates from rebasedMask and legalMask
        for(int j = 0; j<ptrArrLen((void**)rebasedMask); j++){Coordinate_destroy(rebasedMask[j]);}
        for(int j = 0; j<ptrArrLen((void**)legalMask); j++){Coordinate_destroy(legalMask[j]);}
        free(rebasedMask);
        free(legalMask);
        Coordinate_destroy(coord);
    };
    return legalMoves;
}

bool isStranded(struct Board* board, struct Coordinate* pos, bool isActivePiece){
    enum Cell cellType = board->cells[pos->x * board->height + pos->y];
    bool knightLike =  (cellType%2 == 1) != isActivePiece;
    struct Coordinate* currMask = knightLike ? knightMaskCoords : kingMaskCoords;
    struct Move** rebasedMoves = calloc(9,sizeof(struct Move*));
    struct Move** legalMoves = calloc(9,sizeof(struct Move*));
    for(int i = 0; i<8; i++){
        struct Move* currMove = coordToMove(&currMask[i]);
        struct Move* rebasedMove = rebaseMove(pos,currMove);
        bool isInsideBoardX = rebasedMove->to->x >= 0 && rebasedMove->to->x < board->width;
        bool isInsideBoardY = rebasedMove->to->y >= 0 && rebasedMove->to->y < board->height;
        if(isInsideBoardX && isInsideBoardY){
            int cellIndex = rebasedMove->to->x * board->height + rebasedMove->to->y;
            bool isCellBurned = board->cells[cellIndex] == 5;
            if(!isCellBurned){
                legalMoves[ptrArrLen((void**)legalMoves)] = Move_init_copy(rebasedMove);
            }
        }
        Move_destroy(rebasedMove);
        Move_destroy(currMove);
    }
    return ptrArrLen((void**)possibleMoves) == 0;
}

struct GameState* removeStranded(struct GameState* currGame){
    struct GameState* newGame = GameState_init_copy(currGame);
    for(int i = 0; i< currGame->board->width; i++){
        for(int j = 0; j<currGame->board->height; j++){
            struct Coordinate* currPos = Coordinate_init_construct(i,j);
            enum Cell currCell = currGame->board->cells[i * currGame->board->height + j];
            bool isActivePiece = false;
            if(newGame->activePiece != NULL){
                if(i == newGame->activePiece->x && j == newGame->activePiece->y){
                    isActivePiece = true;
                }
            }
            if(isStranded(currGame->board,currPos,isActivePiece)){
                newGame->board->cells[i * newGame->board->height + j] = BurntGround;
            }
            Coordinate_destroy(currPos);
        }
    }
    return newGame;
}

struct GameState* movePiece(struct GameState* input, struct Move* currMove){
    struct GameState* newState = GameState_init_copy(input);
    newState->board->cells[currMove->to->x * input->board->height + currMove->to->y] = newState->board->cells[currMove->from->x * input->board->height + currMove->from->y];
    newState->board->cells[currMove->from->x * input->board->height + currMove->from->y] = BurntGround;
    return newState;
}

struct GameState* transition(struct GameState* currState, struct Move* currMove){
    struct Move** posMoves = possibleMoves(currState);
    bool isValid = false;
    for(int i = 0; i<ptrArrLen((void**)posMoves); i++){
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