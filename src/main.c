#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <utils.h>
#include <mod.h>

int main(){
    while(true){
        printf("%s","--- New Game ---\n");
        struct GameState* game = GameState_init_construct(&defaultBoard, true, NULL);
        while(possibleMoves(game)[0] != NULL){
            char* boardStr = stringifyBoard(game->board);
            printf("%s\n",boardStr);
            free(boardStr);
            
            struct Move** posMoves = possibleMoves(game);
            struct Move* chosenMove = posMoves[rand() % ptrArrLen((void**)posMoves)];
        
            struct GameState* newGame = transition(game,chosenMove);
            GameState_destroy(game);
            game = newGame;

            for(int i = 0; posMoves[i] != NULL; i++){Move_destroy(posMoves[i]);}
            free(posMoves);
        }
        game->turn ? printf("%s","--- Circles Wins ---\n") : printf("%s","--- Crosses Wins ---\n");
    }
}