/* Representation
 * 0 = Empty cell
 * 1 = Friendly (First Player) Knight
 * 2 = Friendly (First Player) King
 * 3 = Enemy Knight
 * 4 = Enemy King
 * 5 = Burnt cell
 */
export type Cell = number;

export type Board = {
    width:  number,
    height: number,
    cells:  Cell[][]
}

export type Coordinate = {x: number,y: number};

/* Representation
 * false = Friendly turn (First Player)
 * true  = Enemy turn
 */
export type GameState = {
    board: Board,
    turn:  boolean,
    activePiece: undefined | Coordinate
};

export type Move = {from: Coordinate,to: Coordinate};

export function numToCoord(input:number[]){
    if(input.length !== 2){throw new Error("numPair had an invalid amount of items")};
    return {x:input[0],y:input[1]}
}
export function coordToMove(input:Coordinate):Move{
    return {from:{x:0,y:0},to:input};
}
export function flipX(input:Coordinate):Coordinate{
    return {x:-input.x,y:input.y}
}
export function flipY(input:Coordinate):Coordinate{
    return {x:input.x,y:-input.y}
}
export function rebaseMove(base:Coordinate,input:Move): Move{
    return {
        from:{
            x: input.from.x + base.x,
            y: input.from.y + base.y
        },
        to:{
            x: input.to.x + base.x,
            y: input.to.y + base.y
        }
    }
}
export function isMoveBlocked(board:Board,move:Move):Boolean{
    if(move.to.x >= board.width || move.to.x < 0 || move.to.y >= board.height || move.to.y < 0){
        return true
    } else {
        if( board.cells[move.to.x][move.to.y] === 5){
            return true
        } else {
            return false
        }
    }
}

export function deduplicateCoords(input:Coordinate[]):Coordinate[]{
    let unique:Coordinate[] = [];
    for(const inputElm of input){
        let isDuplicate = false;
        for(const uniqueElm of unique){
            if(uniqueElm.x === inputElm.x && uniqueElm.y === inputElm.y){
                isDuplicate = true;
                break;
            }
        }
        if(!isDuplicate){
            unique.push(inputElm);
        }
    }
    return unique;
}