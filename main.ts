import { possibleMoves, printBoard ,isMoveValid, transition, GameState } from "./mod.ts";
let game:GameState = {
    board: {
        cells: [
            [1,0,5,2,0,1],
            [0,0,0,0,0,0],
            [0,0,0,0,0,0],
            [0,0,0,3,0,0],
            [3,0,4,4,0,5]
        ],
        width:  6,
        height: 5
    },
    turn: false,
    activePiece: undefined
}

