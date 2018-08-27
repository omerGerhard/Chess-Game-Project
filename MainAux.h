
#ifndef MAINAUX_H_
#define MAINAUX_H_

#include "CHESSGame.h"
#include "settings.h"
#include "SPChessParser.h"
#include "Minimax.h"

typedef enum sp_chess_game_check_winner_message_t{
	CHECK_WHITE_PLAYER,
	CHECK_BLACK_PLAYER,
	CHECKMATE_WHITE_PLAYER,
	CHECKMATE_BLACK_PLAYER,
	DRAW,
	ALL_GOOD
}SP_CHESS_CHECK_WINNER_MESSAGE;


typedef enum sp_chess_game_turn_message_t{
	RETRY,
	CHANGE_TURN
}SP_CHESS_GAME_TURN_MESSAGE;


typedef enum sp_chess_game_error_t{
	MEM_ALLOC,
	SDL_ERROR,
	NONE
}GAME_ERROR;


extern GAME_ERROR er;
extern bool console;

/*
 * return true if the game ends by a checkmate or a draw.
 * otherwise, return false.
 */
bool isGameOver(SP_CHESS_CHECK_WINNER_MESSAGE mes);

/*
 * gets the game 'src' after one of the players played his turn and checks the board
 * for check , checkmate or draw. if none of them occured, the game continues and moves
 * on to the other player's turn.
 * @return
 * CHECK_WHITE_PLAYER - if the white player played his turn and the black king is threatened.
 * CHECK_BLACK_PLAYER - if the black player played his turn and the white king is threatened.
 * CHECKMATE_WHITE_PLAYER - if the white player played his turn and got checkmate
 * CHECKMATE_BLACK_PLAYER - if the black player played his turn and got checkmate
 * DRAW - if one of the players played his turn and the game ends with a draw
 * ALL_GOOD - if there is not a special case and the game can continue to the next turn.
 */
SP_CHESS_CHECK_WINNER_MESSAGE spChessCheckWinner(SPChessGame* src);


/*
 * prints the opening message of the game in console mode
 */
void printConsoleMessage();

/*
 * * execution of the command "undo"
 */
SP_CHESS_GAME_TURN_MESSAGE executeUndo(SPChessGame* src);


/*
 * simulates a computer turn by calling executeComputerMove function
 */
SP_COMMAND playComputerTurn(SPChessGame* src, int difficulty);


/*
 * if the file can be opened, saves the current game settings (from 'settings')
 *  and gameboard (from 'src') to the file 'path'.
 * otherwise, prints an error to the user.
 */
bool saveGame(char* path , SPChessGame* src, Settings* settings);



/*
 * gets a path ('path') to a saved game file.
 * if the file can be opened, loads the saved gameboard to 'src' gameboard and
 * loads the saved settings to the current game settings ('settings').
 * otherwise, prints an error and continue to the next command.
 */
bool loadGame(char* path,SPChessGame* src,Settings* settings);


/*
 * running the program on console mode
 */
void mainConsole();


/*
 * frees the current game settings ('settings') and current game ('src').
 */
void freeResources(SPChessGame* src , Settings *settings);

/*
 * prints an error after a memory allocation problem occured.
 */
void printError(GAME_ERROR er);

/*
 * execution of the command "move"
 */
SP_CHESS_GAME_TURN_MESSAGE executeMove(SPChessGame* src, SPCommand spc);

#endif /* MAINAUX_H_ */
