#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <utils.h>
#include <mod.h>

int main(){
    struct GameState* game = GameState_init_construct(&defaultBoard, false, NULL);
    while(true){
        char* printableString = stringifyBoard(game->board);
        printf("%s\n",printableString);
        free(printableString);
        struct Move** posMoves = possibleMoves(game);
        printMovePtrArr(posMoves);
        int posMovesLength = ptrArrLen((void**)posMoves);
        if(posMovesLength > 0){
            struct GameState* newState = transition(game,posMoves[0]);
            GameState_destroy(game);
            game = newState;
            for(int i=0; i< ptrArrLen((void**)posMoves);i++){Move_destroy(posMoves[i]);}
        } else {
            for(int i=0; i< ptrArrLen((void**)posMoves);i++){Move_destroy(posMoves[i]);}
            break;
        }
    }
    printf("Finished\n");
}