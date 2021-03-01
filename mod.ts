import { t } from "./deps.ts";
import { Board, Move, Coordinate, GameState, coordToMove, numToCoord, flipX, flipY, rebaseMove, isMoveBlocked, deduplicateCoords } from "./utils.ts";
export type { Board, Move, Coordinate, GameState };
const knightMaskCoords: Coordinate[] = [[1,2],[2,1]]
    .map(numToCoord)
    .flatMap(elm=>[elm,flipX(elm)])
    .flatMap(elm=>[elm,flipY(elm)]);
const kingMaskCoords: Coordinate[] = [[1,1],[1,0],[0,1]]
    .map(numToCoord)
    .flatMap(elm=>[elm,flipX(elm)])
    .flatMap(elm=>[elm,flipY(elm)]);

const knightMask = deduplicateCoords(knightMaskCoords).map(coordToMove);
const kingMask = deduplicateCoords(kingMaskCoords).map(coordToMove);

console.log()

export function possibleMoves(currentState: GameState): Move[]{
    let output: Move[] = [];
    if(currentState.activePiece){
        const pieceCoords = currentState.activePiece;
        let possiblyValidMoves: Move[] = [];
        const currCellType = currentState.board.cells[pieceCoords.x][pieceCoords.y];
        possiblyValidMoves.push(...(
            (currCellType %2 === 0 ? knightMask: kingMask).map(
                (move)=>(rebaseMove({x:pieceCoords.x,y:pieceCoords.y},move))
            )
        ));
        possiblyValidMoves = possiblyValidMoves.filter((move)=>(
            !isMoveBlocked(currentState.board,move)
        ));
        output.push(...possiblyValidMoves);
    } else {
        for(let i = 0; i<currentState.board.width; i++){
            for(let j = 0; j<currentState.board.height; j++){
                const currCellType = currentState.board.cells[i][j];
                const matching = currentState.turn ? [3,4] : [1,2];
                let possiblyValidMoves:Move[] = [];
                if(matching.includes(currCellType)){
                    possiblyValidMoves.push(...(
                        (currCellType%2 === 1 ? knightMask: kingMask).map(
                            (move)=>(rebaseMove({x:i,y:j},move))
                        )
                    ));
                };
                possiblyValidMoves = possiblyValidMoves.filter((move)=>(
                    !isMoveBlocked(currentState.board,move)
                ));
                output.push(...possiblyValidMoves);
            }
        }
    }
    return output;
}
export function isMoveValid(state: GameState, move: Move):boolean{
    const validMoves = possibleMoves(state);
    let isValid = false;
    for(const validMove of validMoves){
        let checks: boolean[] = [];
        checks.push(validMove.from.x === move.from.x);
        checks.push(validMove.from.y === move.from.y);
        checks.push(validMove.to  .x === move.to  .x);
        checks.push(validMove.to  .y === move.to  .y);
        if(checks.every(check=>check)){
            isValid = true;
            break;
        }
    }
    return isValid;
}

function removeStranded(currBoard: Board):Board{
    return {...currBoard,cells: currBoard.cells.map((column,i)=>column.map((cell,j)=>{
        if(cell === 0 || cell === 5){
            return cell;
        } else {
            const testTurn = cell === 3 || cell === 4;
            const testMoves = possibleMoves({board: currBoard, turn: testTurn, activePiece: undefined});
            const applicableTestMoves = testMoves.filter(move=>((move.from.x === i) && (move.from.y === j)));
            if(applicableTestMoves.length === 0){
                return 5
            }
            return cell;
        }
    }))};
}

function movePiece(input:Board,currMove:Move):Board{
    const newCells = input.cells.map((column,i)=>column.map((cell,j)=>{
        if((currMove.from.x === i) && (currMove.from.y === j)){
            return 5;
        } else if ((currMove.to.x === i) && (currMove.to.y === j)){
            return input.cells[currMove.from.x][currMove.from.y]
        } else {
            return cell
        }
    }));
    return {...input,cells: newCells};
}

export function transition(currState: GameState, currMove: Move): GameState{
    if(!isMoveValid(currState,currMove)){throw new Error("Move is invalid")};
    
    const destinationCell = currState.board.cells[currMove.to.x][currMove.to.x];
    if(destinationCell === 0){
        return {
            ...currState,
            turn: !currState.turn,
            activePiece: undefined,
            board: removeStranded(movePiece(currState.board,currMove))
        };
    } else {
        return {
            ...currState,
            activePiece: {x: currMove.to.x, y:currMove.to.y},
            board: removeStranded(movePiece(currState.board,currMove))
        }
    }
}



export function printBoard(currBoard: Board): string{
    let output: string = "";
    for(let i=currBoard.height - 1; i>=0; i--){
        for(let j=0; j<currBoard.width; j++){
            const cell = currBoard.cells[j][i];
            switch(cell){
                case 0:
                    output += '░'
                    break
                case 1:
                    output += '×'
                    break
                case 2:
                    output += '+'
                    break
                case 3:
                    output += '⊗'
                    break
                case 4:
                    output += '⊕'
                    break
                case 5:
                    output += '█'
                    break
                default:
                    throw new Error("Unknown cell type");
            }
        }
        output += "\n"
    }
    return output.trim();
}
export const defaultBoard:Board = {
    width: 6,
    height: 5,
    cells: [
        [1,0,0,0,3],
        [0,0,0,0,0],
        [2,0,0,0,4],
        [2,0,0,0,4],
        [0,0,0,0,0],
        [1,0,0,0,3]
    ]
}