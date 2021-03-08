#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <wchar.h>

#include <utils.h>

struct Board* Board_init_new(){
    struct Board* newBoard = calloc(1,sizeof(struct Board));
    newBoard->width = 0;
    newBoard->height = 0;
    newBoard->cells = 0;
    return newBoard;
}
struct Board* Board_init_copy(struct Board* input){
    struct Board* newBoard = calloc(1,sizeof(struct Board));
    newBoard->width = input->width;
    newBoard->height = input->height;
    unsigned int tableSize = input->width*input->height;
    newBoard->cells = calloc(tableSize,sizeof(enum Cell));
    memcpy(newBoard->cells,input,tableSize*sizeof(enum Cell));
    return newBoard;
}
struct Board* Board_init_construct(enum Cell* cells, unsigned int width, unsigned int height){
    struct Board* newBoard = calloc(1,sizeof(struct Board));
    newBoard->width = width;
    newBoard->height = height;
    unsigned int tableSize = width*height;
    newBoard->cells = calloc(tableSize,sizeof(enum Cell));
    memcpy(newBoard->cells,cells,tableSize*sizeof(enum Cell));
    return newBoard;
}
int Board_destroy(struct Board* board){
    free(board->cells);
    free(board);
    return 0;
}

struct Coordinate* Coordinate_init_new(){
    return calloc(1,sizeof(struct Coordinate));
}
struct Coordinate* Coordinate_init_copy(struct Coordinate* input){
    struct Coordinate* newCoord = calloc(1,sizeof(struct Coordinate));
    newCoord->x = input->x;
    newCoord->y = input->y;
    return newCoord;
}
struct Coordinate* Coordinate_init_construct(int x, int y){
    struct Coordinate* newCoord = calloc(1,sizeof(struct Coordinate));
    newCoord->x = x;
    newCoord->y = y;
    return newCoord;
}
int Coordinate_destroy(struct Coordinate* input){
    free(input);
    return 0;
}

struct GameState* GameState_init_new(){
    struct GameState* newGameState = calloc(1,sizeof(struct GameState));
    newGameState->board = Board_init_new();
    newGameState->activePiece = Coordinate_init_new();
    return newGameState;
}
struct GameState* GameState_init_copy(struct GameState* input){
    struct GameState* newGameState = calloc(1,sizeof(struct GameState));
    newGameState->board = Board_init_copy(input->board);
    newGameState->turn = input->turn;
    newGameState->activePiece = Coordinate_init_copy(input->activePiece);
    return newGameState;
}
struct GameState* GameState_init_construct(struct Board* input, bool turn, struct Coordinate* activePiece){
    struct GameState* newGameState = calloc(1,sizeof(struct GameState));
    newGameState->board = Board_init_copy(input);
    newGameState->turn = turn;
    newGameState->activePiece = Coordinate_init_copy(activePiece);
    return newGameState;
}
int GameState_destroy(struct GameState* input){
    Board_destroy(input->board);
    Coordinate_destroy(input->activePiece);
    free(input);
    return 0;
}

struct Move* Move_init_new(){
    struct Move* newMove = calloc(1,sizeof(struct Move));
    newMove->from = Coordinate_init_new();
    newMove->to = Coordinate_init_new();
    return newMove;
}
struct Move* Move_init_copy(struct Move* input){
    struct Move* newMove = calloc(1,sizeof(struct Move));
    newMove->from = Coordinate_init_copy(input->from);
    newMove->to = Coordinate_init_copy(input->to);
    return newMove;
}
struct Move* Move_init_construct(struct Coordinate* from, struct Coordinate* to){
    struct Move* newMove = calloc(1,sizeof(struct Move));
    newMove->from = Coordinate_init_copy(from);
    newMove->to = Coordinate_init_copy(to);
    return newMove;
}
int Move_destroy(struct Move* input){
    Coordinate_destroy(input->from);
    Coordinate_destroy(input->to);
    free(input);
    return 0;
}

struct Coordinate* numToCoord(unsigned int* input){
    return Coordinate_init_construct(input[0],input[1]);
}
struct Move* coordToMove(struct Coordinate* input){
    struct Coordinate* origin = Coordinate_init_construct(0,0);
    struct Move* newMove = Move_init_construct(origin,input);
    Coordinate_destroy(origin);
    return newMove;
}
struct Coordinate* flipX(struct Coordinate* input){
    return Coordinate_init_construct(-input->x,input->y);
}
struct Coordinate* flipY(struct Coordinate* input){
    return Coordinate_init_construct(input->x,-input->y);
}
struct Move* rebaseMove(struct Coordinate* newBase, struct Move* input){
    struct Move* newMove = Move_init_copy(input);
    
    newMove->from->x = newBase->x + input->from->x;
    newMove->from->y = newBase->y + input->from->y;

    newMove->to->x = newBase->x + input->to->x;
    newMove->to->y = newBase->y + input->to->y;

    return newMove;
}
bool isMoveBlocked(struct Board* board,struct Move* move){
    if(move->to->x >= board->width || move->to->x < 0 || move->to->y >= board->height || move->to->y < 0){
        return true;
    } else {
        if(board->cells[(move->to->x)*(board->height) + (move->to->y)] == BurntGround){
            return true;
        } else {
            return false;
        }
    }
}

struct Coordinate** deduplicateCoords(struct Coordinate** input,int inputLen){
    struct Coordinate** unique = calloc(inputLen,sizeof(struct Coordinate*));
    for(int i = 0; i< inputLen; i++){
        bool isDuplicate = false;
        for(int j = 0; j< inputLen; j++){
            if(unique[i] != 0){
                if((unique[j]->x == input[i]->x) && (unique[j]->y == input[i]->y)){
                    isDuplicate = true;
                    break;
                }
            }
        }
        if(!isDuplicate){
            int k;
            for(k = 0; unique[k] != 0; k++){}
            unique[k] = Coordinate_init_copy(input[i]);
        }
    }
    return unique;
}

char* printMove(struct Move* input){
    int stringSize = snprintf(NULL,0,"from:[%d,%d] to:[%d,%d]",input->from->x,input->from->y,input->to->x,input->to->y);
    char* outString = calloc(stringSize+1,sizeof(char));
    sprintf(outString,"from:[%d,%d] to:[%d,%d]",input->from->x,input->from->y,input->to->x,input->to->y);
    return outString;
}

wchar_t* stringifyBoard(struct Board* board){
    wchar_t* output = calloc(((board->width+1) * board->height)+1,sizeof(wchar_t));
    for(int i=board->height - 1; i>=0; i--){
        for(int j=0; j< board->width; j++){
            enum Cell cell = board->cells[(j*board->height) + i];
            switch(cell){
                case Empty:
                    output[wcslen(output)] = L'░';
                    break;
                case CrossesKnight:
                    output[wcslen(output)] = L'×';
                    break;
                case CrossesKing:
                    output[wcslen(output)] = L'+';
                    break;
                case CirclesKnight:
                    output[wcslen(output)] = L'⊗';
                    break;
                case CirclesKing:
                    output[wcslen(output)] = L'⊕';
                    break;
                case BurntGround:
                    output[wcslen(output)] = L'█';
                    break;
                default:
                    printf("Unknown cell type");
            }
        }
        output[wcslen(output)] = L'\n';
    }
    return output;
}

int ptrArrLen(void** ptrArr){
    int i;
    for(i=0;ptrArr[i] != NULL;i++){}
    return i;
}