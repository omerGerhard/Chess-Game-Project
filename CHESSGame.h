#ifndef SPFIARGAME_H_
#define SPFIARGAME_H_


#include <stdio.h>
#include <stdbool.h>
#include "SPArrayList.h"


//Definitions
//white = lowerCase ; black = upperCase
#define SP_CHESS_GAME_N_ROWS 8
#define SP_CHESS_GAME_N_COLUMNS 8
#define SP_CHESS_GAME_WHITE_PAWN 'm'
#define SP_CHESS_GAME_BLACK_PAWN 'M'
#define SP_CHESS_GAME_WHITE_BISHOP 'b'
#define SP_CHESS_GAME_BLACK_BISHOP 'B'
#define SP_CHESS_GAME_WHITE_ROOK 'r'
#define SP_CHESS_GAME_BLACK_ROOK 'R'
#define SP_CHESS_GAME_WHITE_KNIGHT 'n'
#define SP_CHESS_GAME_BLACK_KNIGHT 'N'
#define SP_CHESS_GAME_WHITE_QUEEN 'q'
#define SP_CHESS_GAME_BLACK_QUEEN 'Q'
#define SP_CHESS_GAME_WHITE_KING 'k'
#define SP_CHESS_GAME_BLACK_KING 'K'
#define SP_CHESS_GAME_TIE_SYMBOL '-'
#define SP_CHESS_GAME_EMPTY_ENTRY ' '
#define SP_CHESS_GAME_BLANK_SQUARE '_'
#define SP_CHESS_GAME_BLACK_PLAYER '0'
#define SP_CHESS_GAME_WHITE_PLAYER '1'

typedef struct sp_chess_game_t {
	char gameBoard[SP_CHESS_GAME_N_ROWS][SP_CHESS_GAME_N_COLUMNS];
	char currentPlayer;
	SPArrayList* history;
	bool isWhiteKingThreatened;
	bool isBlackKingThreatened;
	//You May add any fields you like
} SPChessGame;

/**
 * Type used for returning error codes from game functions
 */
typedef enum sp_chess_game_move_message_t {
	SP_CHESS_GAME_INVALID_POSITION,
	SP_CHESS_GAME_INVALID_PIECE,
	SP_CHESS_GAME_ILLEGAL_MOVE,
	SP_CHESS_GAME_SUCCESS,
	SP_CHESS_GAME_KING_WILL_BE_THREATEND,
	SP_CHESS_GAME_KING_STILL_THREATEND
//You may add any message you like
} SP_CHESS_GAME_MOVE_MESSAGE;

typedef enum sp_chess_game_undo_message_t {
	SP_CHESS_GAME_NO_HISTORY,
	SP_CHESS_GAME_UNDO_SUCCESS,
//You may add any message you like
} SP_CHESS_GAME_UNDO_MESSAGE;




/**
 * Creates a new game with a specified history size. The history size is a
 * parameter which specifies the number of previous moves to store. If the number
 * of moves played so far exceeds this parameter, then first moves stored will
 * be discarded in order for new moves to be stored.
 *
 * @historySize - The total number of moves to undo,
 *                a player can undo at most historySizeMoves turns.
 * @return
 * NULL if either a memory allocation failure occurs or historySize <= 0.
 * Otherwise, a new game instant is returned.
 */
SPChessGame* spChessGameCreate(int historySize);

/**
 *	Creates a copy of a given game.
 *	The new copy has the same status as the src game.
 *
 *	@param src - the source game which will be copied
 *	@return
 *	NULL if either src is NULL or a memory allocation failure occurred.
 *	Otherwise, an new copy of the source game is returned.
 */
SPChessGame* spChessGameCopy(SPChessGame* src);

/**
 * frees all memory allocation associated with a given game. If src==NULL
 * the function does nothing.
 *
 * @param src - the source game
 */
void spChessGameDestroy(SPChessGame* src);

/*
 * return the row value of the cell (0 base)
 */
int getRow(SPCell source);

/*
 * return the col value of the cell (0 base)
 */
int getCol(SPCell source);

/*
 * allocate new integers matrix (initialized to zero)
 * with 'numRows' rows and 'numCols' columns.
 */
int** allocateNewMatrix(int numRows, int numCols);

/*
 * frees allocated matrix memory.
 */
void destroyMatrix(int **matrix,int numRows);

/*
 * return the player char of the given tool.
 * blank char if the tool represents an empty slot in the board.
 */
char toolToPlayer(char tool);

/*
 * set all matrix cells to zero.
 */
void setToZero(int **matrix);

/*
 *return 1 if moving the tool in the 'source' cell to the 'target' cell
 *will cause the tool to be thretened by an opponent tool
 *otherwise return 0.
 */
int isThreatend(SPChessGame* src,SPCell source,SPCell target, char player);

/*
 * for tool 'tool' update 'toolMoves' cell value to
 * zero - if the move is not valid
 * one - if the move is valid
 * two - if the move is valid and the tool captures an opponent tool in that cell.
 */
void validMoves(SPChessGame* src,SPCell source,char tool, int** toolMoves);

/*
 * print all the valid moves for the tool in 'source' cell:
 * prints an additional '*' if the move causing the tool to be threatened.
 * prints an additional '^' if the move captures an opponent tool.
 */
void getMoves(SPChessGame* src,SPCell source);


/*
 * checks if a move of the tool from cell 'source' to cell 'target' is a valid move
 * using spChessGameIsValidMove.
 * returns -
 * SP_CHESS_GAME_INVALID_POSITION if the 'source' cell or the 'target' cell are not valid cells.
 * SP_CHESS_GAME_INVALID_PIECE if the 'source' cell contains a tool which not belongs to the
 * current player.
 * SP_CHESS_GAME_ILLEGAL_MOVE if the move is not a valid move.
 * SP_CHESS_GAME_SUCCESS - if the move is valid
 * SP_CHESS_GAME_KING_WILL_BE_THREATEND - if the move is not valid because the current player
 * king will be threatened.
 * SP_CHESS_GAME_KING_STILL_THREATEND - if the move is not valid because before this turn the
 * current player king was threatend and the current move not solving it
 */
SP_CHESS_GAME_MOVE_MESSAGE isValidMove(SPChessGame* src, SPCell source, SPCell target);


/**
 * checks if a move of the tool from cell 'source' to cell 'target' is a valid move
 * returns -
 * SP_CHESS_GAME_INVALID_POSITION if the 'source' cell or the 'target' cell are not valid cells.
 * SP_CHESS_GAME_INVALID_PIECE if the 'source' cell contains a tool which not belongs to the
 * current player.
 * SP_CHESS_GAME_ILLEGAL_MOVE if the move is not a valid move.
 * SP_CHESS_GAME_SUCCESS - if the move is valid
 * SP_CHESS_GAME_KING_WILL_BE_THREATEND - if the move is not valid because the current player
 * king will be threatened.
 * SP_CHESS_GAME_KING_STILL_THREATEND - if the move is not valid because before this turn the
 * current player king was threatend and the current move not solving it
 */
SP_CHESS_GAME_MOVE_MESSAGE spChessGameIsValidMove(SPChessGame* src, SPCell source, SPCell target);

/*
 * return SP_CHESS_GAME_BLACK_PLAYER if 'currentPlayer' is the the white player
 * return SP_CHESS_GAME_WHITE_PLAYER if 'currentPlayer' is the the black player
 */
char changeTurn(char currentPlayer);

/*
 * performs a move for the tool in 'source' cell to 'target' cell
 */
void spChessGameSetMove(SPChessGame* src, SPCell source, SPCell target);

/**
 * undo the previous valid move performed in the game.
 *
 * @return
 * SP_CHESS_GAME_NO_HISTORY       - if the user invoked this function more then
 *                                 historySize in a row, or in case the game just started and
 *                                 the history is empty.
 * SP_CHESS_GAME_UNDO_SUCCESS     - on success.
 */
SP_CHESS_GAME_UNDO_MESSAGE spChessGameUndoPrevMove(SPChessGame* src);

/**
 * sets the cell <row,col> of the gameboard to contain 'tool'
 */
void setCell(SPChessGame* src , int row , int col , char tool);

/**
 * creates the opening board of the game.
 * sets the src->currentPlayer to white.
 * sets the src->isWhiteKingThreatened to false
 * sets the src->isBlackKingThreatened to false
 */
void createBoard(SPChessGame* src);

/*
 * prints the message returned from isValidMove function.
 */
void printMoveMessage(SP_CHESS_GAME_MOVE_MESSAGE mes);


/*
 * used to write the gameboard to the file 'fp' during the operation of 'save'.
 * prints 'row' to the file pointed by 'fp'.
 */
void printRowToFile(FILE* fp, char *row , int rowNum);


/*
 * using printRowToFile to write the current gameboard to the file pointed by 'fp'.
 */
void spChessGamePrintBoardToFile(FILE* fp, SPChessGame* src);



#endif
