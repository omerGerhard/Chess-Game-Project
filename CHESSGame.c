
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "CHESSGame.h"
#include "cell.h"
#include "MainAux.h"
#include "settings.h"


SPChessGame* spChessGameCreate(int historySize){
	if(historySize<=0)
		return NULL;
	SPChessGame *game = (SPChessGame *)malloc(sizeof(SPChessGame));
	if(game==NULL){
		er=MEM_ALLOC;
		return NULL;
	}
	game->history = spArrayListCreate(historySize);
	if(game->history==NULL){
		free(game);
		er=MEM_ALLOC;
		return NULL;
	}
	createBoard(game);
	return game;
}

SPChessGame* spChessGameCopy(SPChessGame* src){
	if(src==NULL)
		return NULL;
	SPChessGame *copy = spChessGameCreate(src->history->maxSize);
	if(copy==NULL){
		er = MEM_ALLOC;
		return NULL;
	}
	spArrayListDestroy(copy->history);
	copy->history = spArrayListCopy(src->history);
	if(copy->history==NULL){
		er = MEM_ALLOC;
		free(copy);
		return NULL;
	}
	copy->currentPlayer = src->currentPlayer;
	for(int i=0; i<SP_CHESS_GAME_N_ROWS ; i++){
		for(int j=0 ; j<SP_CHESS_GAME_N_COLUMNS ; j++)
			copy->gameBoard[i][j] = src->gameBoard[i][j];
	}
	copy->isBlackKingThreatened = src->isBlackKingThreatened;
	copy->isWhiteKingThreatened = src->isWhiteKingThreatened;
	return copy;
}

void spChessGameDestroy(SPChessGame* src){
	if(src!=NULL){
		spArrayListDestroy(src->history);
		free(src);
	}
}

/*
 * return true if the cell is a valid cell of the board.
 * otherwise, return false.
 */
bool isLegalCell(int row , int col){
	return (row>=0 && row<=7 && col>=0 && col<=7);
}

void setCell(SPChessGame* src , int row , int col , char tool){
	src->gameBoard[row][col] = tool;
}


int getRow(SPCell source){
	return source.row-1;
}


int getCol(SPCell source){
	return source.col-'A';
}

/*
 * frees the relevant allocated memory of a matrix in case there
 * was a problem during its allocation.
 */
void destroyMatrixOnFail(int **matrix , int bound){
	for(int i=0; i<bound; i++){
		free(matrix[i]);
	}
	free(matrix);
}


int** allocateNewMatrix(int numRows, int numCols){
	int** matrix = (int **)malloc(numRows*sizeof(int *));
	if(matrix == NULL){
		er = MEM_ALLOC;
		return NULL;
	}
	for(int i=0; i<numRows; i++){
		matrix[i] = (int *)malloc(numCols*sizeof(int));
		if(matrix[i] == NULL){
			er = MEM_ALLOC;
			destroyMatrixOnFail(matrix,i);
			return NULL;
		}
	}
	for(int i=0; i<numRows; i++){
		for(int j=0; j<numCols; j++)
			matrix[i][j]=0;
	}
	return matrix;
}


void destroyMatrix(int **matrix,int numRows){
	for(int i=0; i<numRows; i++)
		free(matrix[i]);
	free(matrix);
}


char toolToPlayer(char tool){
	if(isupper(tool))
		return SP_CHESS_GAME_BLACK_PLAYER;
	else if(islower(tool))
		return SP_CHESS_GAME_WHITE_PLAYER;
	return '_';
}


/*
 * returns 2 if the target is an opponent tool
 * returns 1 if the target is an empty cell
 * otherwise, returns 0
 */
int isEating(char target , char player){
	if(target==SP_CHESS_GAME_BLANK_SQUARE)
		return 1;
	if(player=='1' && isupper(target))
		return 2;
	if(player=='0' && islower(target))
		return 2;
	return 0;
}

/*
 * scan all optional moves for the tool in 'source' cell in a specific direction in the board.
 * the direction defined by 'up' and 'right' -
 * 'up' = -1 (down) / 0 (stay) / 1(up)
 * 'right' = -1(left) / 0(stay) / 1(right)
 */
void scanBoardInDirection(SPChessGame* src,char player,SPCell source,int up,int right,bool finite,bool isPawn,int** matrix){
	int row = getRow(source)+up;
	int col = getCol(source)+right;
	char target , inTheWay;
	int result;
	if(isLegalCell(row,col)){
		target = src->gameBoard[row][col];
		if(finite){
			if(isPawn){
				if(right!=0){
					if(player==SP_CHESS_GAME_WHITE_PLAYER)
						result = isupper(target)? 2 : 0;
					else
						result = islower(target)? 2 : 0;
				}
				else{
					if(up == 2){
						inTheWay = src->gameBoard[row-1][col];
						result = ((target==SP_CHESS_GAME_BLANK_SQUARE) &&
								(inTheWay==SP_CHESS_GAME_BLANK_SQUARE));
					}
					else if(up == -2){
						inTheWay = src->gameBoard[row+1][col];
						result = ((target==SP_CHESS_GAME_BLANK_SQUARE) &&
								(inTheWay==SP_CHESS_GAME_BLANK_SQUARE));
					}
					else
						result = (target==SP_CHESS_GAME_BLANK_SQUARE);
				}
			}
			else{
				result = isEating(target,player);
			}
			if(result!=0)
				matrix[row][col]=result;
		}
		else{
			for(;isLegalCell(row,col);row+=up,col+=right){
				target = src->gameBoard[row][col];
				if(target==SP_CHESS_GAME_BLANK_SQUARE)
					matrix[row][col]=1;
				else{
					if(player==SP_CHESS_GAME_WHITE_PLAYER)
						result = isupper(target)? 2 : 0;
					else
						result = islower(target)? 2 : 0;
					if(result!=0)
						matrix[row][col]=result;
					break;
				}
			}
		}
	}
}


/*
 * return matrix which contains 1's in every reachable cell for the white pawn
 * from cell 'source'
 */
void checkWhitePawnMove(SPChessGame* src, SPCell source, int** matrix){
	int row = getRow(source);
	scanBoardInDirection(src,SP_CHESS_GAME_WHITE_PLAYER,source,1,0,true,true,matrix);
	if(row==1)
		scanBoardInDirection(src,SP_CHESS_GAME_WHITE_PLAYER,source,2,0,true,true,matrix);
	scanBoardInDirection(src,SP_CHESS_GAME_WHITE_PLAYER,source,1,1,true,true,matrix);
	scanBoardInDirection(src,SP_CHESS_GAME_WHITE_PLAYER,source,1,-1,true,true,matrix);
}

/*
 * return matrix which contains 1's in every reachable cell for the black pawn
 * from cell 'source'
 */
void checkBlackPawnMove(SPChessGame* src, SPCell source, int** matrix){
	int row = getRow(source);
	scanBoardInDirection(src,SP_CHESS_GAME_BLACK_PLAYER,source,-1,0,true,true,matrix);
	if(row==6)
		scanBoardInDirection(src,SP_CHESS_GAME_BLACK_PLAYER,source,-2,0,true,true,matrix);
	scanBoardInDirection(src,SP_CHESS_GAME_BLACK_PLAYER,source,-1,1,true,true,matrix);
	scanBoardInDirection(src,SP_CHESS_GAME_BLACK_PLAYER,source,-1,-1,true,true,matrix);
}

/*
 * return matrix which contains 1's in every reachable cell for the bishop
 * from cell 'source'
 */
void checkBishopMove(SPChessGame* src,char player, SPCell source, int** matrix){
	scanBoardInDirection(src,player,source,1,1,false,false,matrix);
	scanBoardInDirection(src,player,source,1,-1,false,false,matrix);
	scanBoardInDirection(src,player,source,-1,1,false,false,matrix);
	scanBoardInDirection(src,player,source,-1,-1,false,false,matrix);
}

/*
 * return matrix which contains 1's in every reachable cell for the rook
 * from cell 'source'
 */
void checkRookMove(SPChessGame* src,char player, SPCell source, int** matrix){
	scanBoardInDirection(src,player,source,1,0,false,false,matrix);
	scanBoardInDirection(src,player,source,-1,0,false,false,matrix);
	scanBoardInDirection(src,player,source,0,1,false,false,matrix);
	scanBoardInDirection(src,player,source,0,-1,false,false,matrix);
}

/*
 * return matrix which contains 1's in every reachable cell for the knight
 * from cell 'source'
 */
void checkKnightMove(SPChessGame* src,char player, SPCell source, int** matrix){
	scanBoardInDirection(src,player,source,2,1,true,false,matrix);
	scanBoardInDirection(src,player,source,2,-1,true,false,matrix);
	scanBoardInDirection(src,player,source,-2,1,true,false,matrix);
	scanBoardInDirection(src,player,source,-2,-1,true,false,matrix);
	scanBoardInDirection(src,player,source,1,2,true,false,matrix);
	scanBoardInDirection(src,player,source,1,-2,true,false,matrix);
	scanBoardInDirection(src,player,source,-1,2,true,false,matrix);
	scanBoardInDirection(src,player,source,-1,-2,true,false,matrix);
}

/*
 * return matrix which contains 1's in every reachable cell for the queen
 * from cell 'source'
 */
void checkQueenMove(SPChessGame* src,char player,SPCell source, int** matrix){
	scanBoardInDirection(src,player,source,1,0,false,false,matrix);
	scanBoardInDirection(src,player,source,1,1,false,false,matrix);
	scanBoardInDirection(src,player,source,0,1,false,false,matrix);
	scanBoardInDirection(src,player,source,-1,1,false,false,matrix);
	scanBoardInDirection(src,player,source,-1,0,false,false,matrix);
	scanBoardInDirection(src,player,source,-1,-1,false,false,matrix);
	scanBoardInDirection(src,player,source,0,-1,false,false,matrix);
	scanBoardInDirection(src,player,source,1,-1,false,false,matrix);
}

/*
 * return matrix which contains 1's in every reachable cell for the king
 * from cell 'source'
 */
void checkKingMove(SPChessGame* src,char player, SPCell source, int** matrix){
	scanBoardInDirection(src,player,source,1,0,true,false,matrix);
	scanBoardInDirection(src,player,source,1,1,true,false,matrix);
	scanBoardInDirection(src,player,source,0,1,true,false,matrix);
	scanBoardInDirection(src,player,source,-1,1,true,false,matrix);
	scanBoardInDirection(src,player,source,-1,0,true,false,matrix);
	scanBoardInDirection(src,player,source,-1,-1,true,false,matrix);
	scanBoardInDirection(src,player,source,0,-1,true,false,matrix);
	scanBoardInDirection(src,player,source,1,-1,true,false,matrix);
}


/*
 * updates the matrix 'matrix' with all the reachable cells
 * for the tool 'tool' to move to in the board.
 */
void updateToolMove(SPChessGame* src,SPCell source,char tool, int** matrix){
		char player = toolToPlayer(tool);
		if(tool=='M')
			checkBlackPawnMove(src, source, matrix);
		else if(tool=='m')
			checkWhitePawnMove(src, source, matrix);
		else if(tool=='N' || tool=='n')
			checkKnightMove(src,player, source, matrix);
		else if(tool=='B' || tool=='b')
			checkBishopMove(src,player, source, matrix);
		else if(tool=='R' || tool=='r')
			checkRookMove(src,player, source, matrix);
		else if(tool=='Q' || tool=='q')
			checkQueenMove(src,player, source, matrix);
		else if(tool=='K' || tool=='k')
			checkKingMove(src,player, source, matrix);
}

/*
 * updates the matrix 'matrix' with all the reachable cells of the board that
 * an opponent tool can move to.
 */
void checkOpponentMoves(SPChessGame* src,int** matrix,char player){
	char tool;
	SPCell cell;
	for(int i=0; i<SP_CHESS_GAME_N_ROWS; i++){
		for(int j=0; j<SP_CHESS_GAME_N_COLUMNS; j++){
			tool = src->gameBoard[i][j];
			cell = spCellCreate(i+1,j+'A');
			if(player=='1' && isupper(tool)){
				updateToolMove(src,cell,tool,matrix);
			}
			else if(player=='0' && islower(tool)){
				updateToolMove(src,cell,tool,matrix);
			}
		}
	}
}

/*
 * print the cell in row 'row' and column 'col' as optional target cell for player move
 * with '*' if the move to that cell will cause the tool to be threatened
 * and '^' if the move to that cell will eat an opponent tool.
 */
void printCell(int threat , int capt, int row , int col){
	printf("<%d,%c>",row+1,col+'A');
	if(threat)
		printf("*");
	if(capt)
		printf("^");
	printf("\n");
}

void setToZero(int **matrix){
	for(int i=0; i<SP_CHESS_GAME_N_ROWS; i++){
		for(int j=0; j<SP_CHESS_GAME_N_COLUMNS; j++){
			matrix[i][j]=0;
		}
	}
}

int isThreatend(SPChessGame* src,SPCell source,SPCell target, char player){
	SPChessGame* copy = spChessGameCopy(src);
	if(er != NONE)
		return 0;
	spChessGameSetMove(copy,source,target);
	if(er != NONE){
		spChessGameDestroy(copy);
		return 0;
	}
	char tool;
	player = changeTurn(player);
	int targetRow = getRow(target),targetCol = getCol(target);
	SPCell cell;
	int** mid_matrix = allocateNewMatrix(8,8);
	if(er != NONE){
		spChessGameDestroy(copy);
		return 0;
	}
	for(int i=0; i<SP_CHESS_GAME_N_ROWS; i++){
		for(int j=0; j<SP_CHESS_GAME_N_COLUMNS; j++){
			tool = copy->gameBoard[i][j];
			if(player==toolToPlayer(tool)){
				cell = spCellCreate(i+1,j+'A');
				validMoves(copy,cell,tool,mid_matrix);
				if(er != NONE){
					destroyMatrix(mid_matrix,8);
					spChessGameDestroy(copy);
					return 0;
				}
				if(mid_matrix[targetRow][targetCol]==2){
					destroyMatrix(mid_matrix,8);
					spChessGameDestroy(copy);
					return 1;
				}
				setToZero(mid_matrix);
			}
		}
	}
	destroyMatrix(mid_matrix,8);
	spChessGameDestroy(copy);
	return 0;
}

/*
 *print the moves in 'get_moves' format for the tool in 'source' cell.
 */
void printMoves(SPChessGame* src,SPCell source,int** toolMoves){
	int threat , capt, sourceRow = getRow(source), sourceCol = getCol(source);
	SPCell target;
	char player = toolToPlayer(src->gameBoard[sourceRow][sourceCol]);
	for(int i=0; i<SP_CHESS_GAME_N_ROWS; i++){
		for(int j=0; j<SP_CHESS_GAME_N_COLUMNS; j++){
			if(toolMoves[i][j]!=0){
				target = spCellCreate(i+1,j+'A');
				threat = isThreatend(src,source,target,player);
				if(er != NONE)
					return;
				capt = toolMoves[i][j]==2 ? 1 : 0;
				printCell(threat,capt,i,j);
			}
		}
	}
}

/*
 *for every cell in 'matrix' which is not zero
 *checks if the move is valid - if it is not, change the value in the cell to zero.
 */
void updateValidMoves(SPChessGame* src,SPCell source, int** matrix){
	SP_CHESS_GAME_MOVE_MESSAGE mes;
	SPCell target;
	for(int i=0; i<SP_CHESS_GAME_N_ROWS; i++){
		for(int j=0; j<SP_CHESS_GAME_N_COLUMNS; j++){
			if(matrix[i][j]!=0){
				target = spCellCreate(i+1,j+'A');
				mes =  spChessGameIsValidMove(src,source,target);
				if(er != NONE)
					return;
				if(mes!=SP_CHESS_GAME_SUCCESS)
					matrix[i][j]=0;
			}
		}
	}
}


void validMoves(SPChessGame* src,SPCell source,char tool, int** toolMoves){
	updateToolMove(src,source,tool,toolMoves);
	updateValidMoves(src,source,toolMoves);
}


void getMoves(SPChessGame* src,SPCell source)
{
	int row = getRow(source);
	int col = getCol(source);
	char tool = src->gameBoard[row][col];
	if(tool==SP_CHESS_GAME_BLANK_SQUARE){
		printf("The specified position does not contain a player piece\n");
		return;
	}
	int** toolMoves = allocateNewMatrix(8,8);
	if(er != NONE)
		return;
	validMoves(src,source,tool,toolMoves);
	if(er != NONE){
		destroyMatrix(toolMoves,8);
		return;
	}
	printMoves(src,source,toolMoves);
	destroyMatrix(toolMoves,8);
}

/*
 * return the cell of the king belongs to 'player' player
 */
SPCell findKing(SPChessGame* src , char player){
	SPCell king = spCellCreate(0,'A');
	//char player = src->currentPlayer;
	char searchedKing = (player==SP_CHESS_GAME_WHITE_PLAYER) ? 'k' : 'K';
	for(int i=0; i<SP_CHESS_GAME_N_ROWS; i++){
		for(int j=0; j<SP_CHESS_GAME_N_COLUMNS; j++){
			if(src->gameBoard[i][j]==searchedKing){
				king = spCellCreate(i+1,j+'A');
				return king;
			}
		}
	}
	return king;
}

/*
 * return true if the king belongs to player 'player' is threatendd.
 * otherwise, return false
 */
bool kingStatus(SPChessGame* src, char player){
	if(player == SP_CHESS_GAME_WHITE_PLAYER)
		return src->isWhiteKingThreatened;
	return src->isBlackKingThreatened;
}

/*
 * return true if the king belongs to player 'player' is threatend by an opponent tool.
 * otherwise, return false.
 * used to test if a specific move is valid.
 */
bool isKingThreatend(SPChessGame* src,char player){
	int **checkOpponent = allocateNewMatrix(8,8),kingCell;
	if(er != NONE)
		return false;
	checkOpponentMoves(src,checkOpponent,player);
	SPCell kingLoc = findKing(src , player);
	if(kingLoc.row==0)
		return false;
	kingCell = checkOpponent[getRow(kingLoc)][getCol(kingLoc)];
	destroyMatrix(checkOpponent,8);
	return kingCell==2;
}


SP_CHESS_GAME_MOVE_MESSAGE isValidMove(SPChessGame* src, SPCell source, SPCell target){
	int row = getRow(source),col = getCol(source);
	char tool = src->gameBoard[row][col];
	char player = src->currentPlayer;
	if((islower(tool) && player=='0')||(isupper(tool) && player=='1'))
		return SP_CHESS_GAME_INVALID_PIECE;
	return spChessGameIsValidMove(src, source, target);
}


SP_CHESS_GAME_MOVE_MESSAGE spChessGameIsValidMove(SPChessGame* src, SPCell source, SPCell target){
	int row = getRow(source),col = getCol(source);
	bool kingBefore , kingAfter;
	int target_row= getRow(target), target_col=getCol(target);
	char tool = src->gameBoard[row][col];
	char player = toolToPlayer(tool);
	SPChessGame* copy;
	int **matrix = allocateNewMatrix(8,8),available;
	if(er != NONE)
		return SP_CHESS_GAME_INVALID_POSITION;
	updateToolMove(src,source,tool,matrix);
	available=matrix[getRow(target)][getCol(target)];
	destroyMatrix(matrix,8);
	if(available){
		copy = spChessGameCopy(src);
		if(er != NONE)
			return SP_CHESS_GAME_INVALID_POSITION;
		kingBefore = kingStatus(copy,player);
		setCell(copy , row , col , SP_CHESS_GAME_BLANK_SQUARE);
		setCell(copy , target_row , target_col , tool);
		kingAfter = isKingThreatend(copy,player);
		spChessGameDestroy(copy);
		if(er != NONE)
			return SP_CHESS_GAME_INVALID_POSITION;
		if(kingAfter){
			if(kingBefore)
				return SP_CHESS_GAME_KING_STILL_THREATEND;
			else
				return SP_CHESS_GAME_KING_WILL_BE_THREATEND;
		}
		else
			return SP_CHESS_GAME_SUCCESS;
	}
	else
		return SP_CHESS_GAME_ILLEGAL_MOVE;
}


char changeTurn(char currentPlayer){
	if(currentPlayer == SP_CHESS_GAME_BLACK_PLAYER)
		return SP_CHESS_GAME_WHITE_PLAYER;
	return SP_CHESS_GAME_BLACK_PLAYER;
}

/*
 * update the status of the king belongs to player 'player' by calling the function
 * isKingThreatend with the right params.
 */


void updateKingStatus(SPChessGame* src){
	src->isWhiteKingThreatened = isKingThreatend(src, '1');
	src->isBlackKingThreatened = isKingThreatend(src, '0');
}


void spChessGameSetMove(SPChessGame* src, SPCell source, SPCell target){
	int src_row = getRow(source), src_col=getCol(source);
	int tar_row = getRow(target), tar_col=getCol(target);
	char tool = src->gameBoard[src_row][src_col];
	char eatenTool = src->gameBoard[tar_row][tar_col];
	//char player;
	SPArrayCell ac = spArrayCellCreate(src_row+1,src_col+'A',tar_row+1,tar_col+'A',tool,eatenTool);
	spArrayListAddLast(src->history,ac);
	setCell(src,src_row,src_col,SP_CHESS_GAME_BLANK_SQUARE);
	setCell(src,tar_row,tar_col,tool);
	src->currentPlayer=changeTurn(src->currentPlayer);
	//player = src->currentPlayer;
	//updateKingStatus(src, player);
	updateKingStatus(src);
	//printf("is black king threatend - %d\n",src->isBlackKingThreatened);
}


SP_CHESS_GAME_UNDO_MESSAGE spChessGameUndoPrevMove(SPChessGame* src){
	if(src->history->actualSize==0)
		return SP_CHESS_GAME_NO_HISTORY;
	int index = src->history->actualSize-1;
	SPCell source = src->history->elements[index].source;
	SPCell target = src->history->elements[index].target;
	char eatenTool = src->history->elements[index].eatenTool;
	char movedTool = src->history->elements[index].tool;
	setCell(src,getRow(source),getCol(source),movedTool);
	setCell(src,getRow(target),getCol(target),eatenTool);
	spArrayListRemoveLast(src->history);
	src->currentPlayer=changeTurn(src->currentPlayer);
	updateKingStatus(src);
	if(er != NONE)
		return SP_CHESS_GAME_NO_HISTORY;
	if(console){
		if(src->currentPlayer==SP_CHESS_GAME_BLACK_PLAYER)
			printf("Undo move for black player: <%d,%c> -> <%d,%c>\n",source.row,source.col,target.row,target.col);
		else
			printf("Undo move for white player: <%d,%c> -> <%d,%c>\n",source.row,source.col,target.row,target.col);
	}
	return SP_CHESS_GAME_UNDO_SUCCESS;
}


void createBoard(SPChessGame* src){
	if(src==NULL)
		return;
	setCell(src,0,0,SP_CHESS_GAME_WHITE_ROOK);
	setCell(src,0,1,SP_CHESS_GAME_WHITE_KNIGHT);
	setCell(src,0,2,SP_CHESS_GAME_WHITE_BISHOP);
	setCell(src,0,3,SP_CHESS_GAME_WHITE_QUEEN);
	setCell(src,0,4,SP_CHESS_GAME_WHITE_KING);
	setCell(src,0,5,SP_CHESS_GAME_WHITE_BISHOP);
	setCell(src,0,6,SP_CHESS_GAME_WHITE_KNIGHT);
	setCell(src,0,7,SP_CHESS_GAME_WHITE_ROOK);
	for(int i=0;i<SP_CHESS_GAME_N_COLUMNS ; i++)
		src->gameBoard[1][i] = 'm';
	for(int i=2 ; i<SP_CHESS_GAME_N_ROWS -2 ; i++){
		for(int j=0 ; j<SP_CHESS_GAME_N_COLUMNS ; j++)
			src->gameBoard[i][j] = SP_CHESS_GAME_BLANK_SQUARE;
	}
	for(int i=0;i<SP_CHESS_GAME_N_COLUMNS ; i++)
		src->gameBoard[6][i] = 'M';
	setCell(src,7,0,SP_CHESS_GAME_BLACK_ROOK);
	setCell(src,7,1,SP_CHESS_GAME_BLACK_KNIGHT);
	setCell(src,7,2,SP_CHESS_GAME_BLACK_BISHOP);
	setCell(src,7,3,SP_CHESS_GAME_BLACK_QUEEN);
	setCell(src,7,4,SP_CHESS_GAME_BLACK_KING);
	setCell(src,7,5,SP_CHESS_GAME_BLACK_BISHOP);
	setCell(src,7,6,SP_CHESS_GAME_BLACK_KNIGHT);
	setCell(src,7,7,SP_CHESS_GAME_BLACK_ROOK);
	src-> currentPlayer = SP_CHESS_GAME_WHITE_PLAYER;
	src->isBlackKingThreatened=false;
	src->isWhiteKingThreatened=false;
	src->history->actualSize = 0;
}

/*
 * prints the matrix 'matrix'.
 */
void printMatrix(int** matrix){
	for(int i=7; i>=0; i--){
		for(int j=0; j<8;j++)
			printf("%d ", matrix[i][j]);
		printf("\n");
	}
	printf("\n");
}


void printMoveMessage(SP_CHESS_GAME_MOVE_MESSAGE mes){
	switch(mes){
		case SP_CHESS_GAME_INVALID_POSITION:
			printf("Invalid position on the board\n");
			break;
		case SP_CHESS_GAME_INVALID_PIECE:
			printf("The specified position does not contain your piece\n");
			break;
		case SP_CHESS_GAME_ILLEGAL_MOVE:
			printf("Illegal move\n");
			break;
		case SP_CHESS_GAME_KING_STILL_THREATEND:
			printf("Illegal move: king is still threatened\n");
			break;
		case SP_CHESS_GAME_KING_WILL_BE_THREATEND:
			printf("Illegal move: king will be threatened\n");
			break;
		default:
			break;
	}
}


void printRowToFile(FILE* fp, char *row , int rowNum){
	fprintf(fp,"%d| ",rowNum);
	for(int i=0 ; i<SP_CHESS_GAME_N_COLUMNS ; i++){
		switch(row[i]){
			case SP_CHESS_GAME_BLANK_SQUARE:
				fprintf(fp,"%c ",SP_CHESS_GAME_BLANK_SQUARE);
				break;
			case SP_CHESS_GAME_WHITE_PAWN:
				fprintf(fp,"%c ",SP_CHESS_GAME_WHITE_PAWN);
				break;
			case SP_CHESS_GAME_BLACK_PAWN:
				fprintf(fp,"%c ",SP_CHESS_GAME_BLACK_PAWN);
				break;
			case SP_CHESS_GAME_WHITE_BISHOP:
				fprintf(fp,"%c ",SP_CHESS_GAME_WHITE_BISHOP);
				break;
			case SP_CHESS_GAME_BLACK_BISHOP:
				fprintf(fp,"%c ",SP_CHESS_GAME_BLACK_BISHOP);
				break;
			case SP_CHESS_GAME_WHITE_ROOK:
				fprintf(fp,"%c ",SP_CHESS_GAME_WHITE_ROOK);
				break;
			case SP_CHESS_GAME_BLACK_ROOK:
				fprintf(fp,"%c ",SP_CHESS_GAME_BLACK_ROOK);
				break;
			case SP_CHESS_GAME_WHITE_KNIGHT:
				fprintf(fp,"%c ",SP_CHESS_GAME_WHITE_KNIGHT);
				break;
			case SP_CHESS_GAME_BLACK_KNIGHT:
				fprintf(fp,"%c ",SP_CHESS_GAME_BLACK_KNIGHT);
				break;
			case SP_CHESS_GAME_WHITE_QUEEN:
				fprintf(fp,"%c ",SP_CHESS_GAME_WHITE_QUEEN);
				break;
			case SP_CHESS_GAME_BLACK_QUEEN:
				fprintf(fp,"%c ",SP_CHESS_GAME_BLACK_QUEEN);
				break;
			case SP_CHESS_GAME_WHITE_KING:
				fprintf(fp,"%c ",SP_CHESS_GAME_WHITE_KING);
				break;
			case SP_CHESS_GAME_BLACK_KING:
				fprintf(fp,"%c ",SP_CHESS_GAME_BLACK_KING);
				break;
			default:
				fprintf(fp,"%c ",SP_CHESS_GAME_EMPTY_ENTRY);
				break;
		}
	}
	fprintf(fp,"|\n");
}



void spChessGamePrintBoardToFile(FILE* fp, SPChessGame* src){
	for(int i=SP_CHESS_GAME_N_ROWS-1 ; i>=0 ; i--)
		printRowToFile(fp,src->gameBoard[i],i+1);
	fprintf(fp,"  "); // 2 spaces before the 17 dashes
	for(int i=0; i<17 ; i++)
		fprintf(fp,"%c",SP_CHESS_GAME_TIE_SYMBOL);
	fprintf(fp,"\n");
	fprintf(fp,"   A B C D E F G H\n"); // 3 spaces before A-H letters
}




