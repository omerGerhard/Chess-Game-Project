


#include "MainAux.h"
#include "CHESSGame.h"
#include "SPChessParser.h"
#include "settings.h"
#include "Minimax.h"
#include <stdio.h>

/*
 * prints the 'rowNum' row of the gameboard.
 */
void printRow(char *row , int rowNum){
	printf("%d| ",rowNum);
	for(int i=0 ; i<SP_CHESS_GAME_N_COLUMNS ; i++){
		switch(row[i]){
			case SP_CHESS_GAME_BLANK_SQUARE:
				printf("%c ",SP_CHESS_GAME_BLANK_SQUARE);
				break;
			case SP_CHESS_GAME_WHITE_PAWN:
				printf("%c ",SP_CHESS_GAME_WHITE_PAWN);
				break;
			case SP_CHESS_GAME_BLACK_PAWN:
				printf("%c ",SP_CHESS_GAME_BLACK_PAWN);
				break;
			case SP_CHESS_GAME_WHITE_BISHOP:
				printf("%c ",SP_CHESS_GAME_WHITE_BISHOP);
				break;
			case SP_CHESS_GAME_BLACK_BISHOP:
				printf("%c ",SP_CHESS_GAME_BLACK_BISHOP);
				break;
			case SP_CHESS_GAME_WHITE_ROOK:
				printf("%c ",SP_CHESS_GAME_WHITE_ROOK);
				break;
			case SP_CHESS_GAME_BLACK_ROOK:
				printf("%c ",SP_CHESS_GAME_BLACK_ROOK);
				break;
			case SP_CHESS_GAME_WHITE_KNIGHT:
				printf("%c ",SP_CHESS_GAME_WHITE_KNIGHT);
				break;
			case SP_CHESS_GAME_BLACK_KNIGHT:
				printf("%c ",SP_CHESS_GAME_BLACK_KNIGHT);
				break;
			case SP_CHESS_GAME_WHITE_QUEEN:
				printf("%c ",SP_CHESS_GAME_WHITE_QUEEN);
				break;
			case SP_CHESS_GAME_BLACK_QUEEN:
				printf("%c ",SP_CHESS_GAME_BLACK_QUEEN);
				break;
			case SP_CHESS_GAME_WHITE_KING:
				printf("%c ",SP_CHESS_GAME_WHITE_KING);
				break;
			case SP_CHESS_GAME_BLACK_KING:
				printf("%c ",SP_CHESS_GAME_BLACK_KING);
				break;
			default:
				printf("%c ",SP_CHESS_GAME_EMPTY_ENTRY);
				break;
		}
	}
	printf("|\n");
}

/*
 * prints the gameboard of the current game 'src' by calling printRow function.
 */
void spChessGamePrintBoard(SPChessGame* src){
	for(int i=SP_CHESS_GAME_N_ROWS-1 ; i>=0 ; i--)
		printRow(src->gameBoard[i],i+1);
	printf("  "); // 2 spaces before the 17 dashes
	for(int i=0; i<17 ; i++)
		printf("%c",SP_CHESS_GAME_TIE_SYMBOL);
	printf("\n");
	printf("   A B C D E F G H\n"); // 3 spaces before A-H letters
}


void printConsoleMessage(){
	printf(" Chess\n");
	printf("-------\n");
}


bool isGameOver(SP_CHESS_CHECK_WINNER_MESSAGE mes){
	if(mes==DRAW || mes==CHECKMATE_WHITE_PLAYER || mes==CHECKMATE_BLACK_PLAYER)
		return true;
	return false;
}

/*
 * return true if the tool 'tool' from 'source' cell has a valid
 * move in the current game 'src'.
 * otherwise, return false.
 */
bool hasMoves(SPChessGame* src , SPCell source, char tool){
	int **matrix = allocateNewMatrix(8,8);
	if(er != NONE)
		return false;
	validMoves(src,source,tool,matrix);
	if(er != NONE){
		destroyMatrix(matrix,8);
		return false;
	}
	for(int i=0; i<SP_CHESS_GAME_N_ROWS; i++){
		for(int j=0; j<SP_CHESS_GAME_N_COLUMNS; j++){
			if(matrix[i][j]!=0){
				destroyMatrix(matrix,8);
				return true;
			}
		}
	}
	destroyMatrix(matrix,8);
	return false;
}

/*
 * prints a message according to the result of checkWinner function.
 * if there is a check of checkmate in the current gameboard, the function prints
 * the appropriate message according to the player that caused it.
 * if there is a draw in the current gameboard, the function prints that the
 * game ends in a draw.
 */
void printWinnerMessage(SP_CHESS_GAME_MOVE_MESSAGE mes){
	switch(mes){
	case DRAW:
		printf("The game ends in a draw\n");
		break;
	case CHECKMATE_BLACK_PLAYER:
		printf("Checkmate! black player wins the game\n");
		break;
	case CHECKMATE_WHITE_PLAYER:
		printf("Checkmate! white player wins the game\n");
		break;
	case CHECK_BLACK_PLAYER:
		printf("Check: white king is threatened\n");
		break;
	case CHECK_WHITE_PLAYER:
		printf("Check: black king is threatened\n");
		break;
	default:
		break;
	}
}


SP_CHESS_CHECK_WINNER_MESSAGE spChessCheckWinner(SPChessGame* src){
	char player = src->currentPlayer,tool;
	bool hasMove;
	SPCell source;
	for(int i=0; i<SP_CHESS_GAME_N_ROWS; i++){
		for(int j=0; j<SP_CHESS_GAME_N_COLUMNS; j++){
			tool=src->gameBoard[i][j];
			source = spCellCreate(i+1,j+'A');
			if(player==toolToPlayer(tool)){
				hasMove = hasMoves(src,source,tool);
				if(er != NONE)
					return ALL_GOOD;
				if(hasMove){
					if(player==SP_CHESS_GAME_BLACK_PLAYER){
						if(src->isBlackKingThreatened)
							return CHECK_WHITE_PLAYER;
						else
							return ALL_GOOD;
					}
					else{
						if(src->isWhiteKingThreatened)
							return CHECK_BLACK_PLAYER;
						else
							return ALL_GOOD;
					}
				}
			}
		}
	}
	if(player==SP_CHESS_GAME_BLACK_PLAYER){
		if(src->isBlackKingThreatened)
			return CHECKMATE_WHITE_PLAYER;
		else
			return DRAW;
	}
	else{
		if(src->isWhiteKingThreatened)
			return CHECKMATE_BLACK_PLAYER;
		else
			return DRAW;
	}
}


/*
 * gets a pointer to a file ('fp') which contains a saved game, parses the saved gameboard
 * from it and updates the current game ('src') gameboard according to it.
 */
void loadBoard(FILE *fp,SPChessGame* src){
	char temp[80];
	for(int i=7; i>=0; i--){
		fgets(temp,80,fp);
		for(int j=0; j<8; j++){
			src->gameBoard[i][j]=temp[(2*j)+3];
		}
	}
}


bool loadGame(char* path,SPChessGame* src,Settings* settings){
	FILE* fp = fopen(path,"r");
	if(fp==NULL){
		printf("Error: File doesn’t exist or cannot be opened\n");
		return false;
	}
	char temp[80];
	fgets(temp,80,fp); //read current player line
	if(temp[0]=='b')
		src->currentPlayer=SP_CHESS_GAME_BLACK_PLAYER;
	else
		src->currentPlayer=SP_CHESS_GAME_WHITE_PLAYER;
	fgets(temp,80,fp); //read SETTINGS: line
	fgets(temp,80,fp); //read game_mode line
	settings->game_mode = getModeFromFile(temp);
	if(settings->game_mode==1){
		fgets(temp,80,fp); //read difficulty line
		settings->difficulty = getDifficultyFromFile(temp);
		fgets(temp,80,fp); //read user_color line
		settings->user_color = getUserColorFromFile(temp);
	}
	loadBoard(fp,src);
	src->history->actualSize = 0;
	fclose(fp);
	return true;
}


bool saveGame(char* path , SPChessGame* src, Settings* settings){
	FILE* fp = fopen(path,"w");
	if(fp==NULL){
		if(console)
			printf("File cannot be created or modified\n");
		else
			printf("ERROR: File cannot be created or modified\n");
		return false;
	}
	char player = src->currentPlayer;
	if(player==SP_CHESS_GAME_WHITE_PLAYER)
		fprintf(fp,"white\n");
	else
		fprintf(fp,"black\n");
	fprintf(fp,"SETTINGS:\n");
	if(settings->game_mode==1){
		fprintf(fp,"GAME_MODE: 1-player\n");
		switch(settings->difficulty){
			case(1):
				fprintf(fp,"DIFFICULTY: amateur\n");
				break;
			case(2):
				fprintf(fp,"DIFFICULTY: easy\n");
				break;
			case(3):
				fprintf(fp,"DIFFICULTY: moderate\n");
				break;
			case(4):
				fprintf(fp,"DIFFICULTY: hard\n");
				break;
			default:
				fprintf(fp,"DIFFICULTY: expert\n");
				break;
			}
		if(settings->user_color==0)
			fprintf(fp,"USER_COLOR: black\n");
		else
			fprintf(fp,"USER_COLOR: white\n");
	}
	else
		fprintf(fp,"GAME_MODE: 2-player\n");
	spChessGamePrintBoardToFile(fp,src);
	fclose(fp);
	if(console)
		printf("Game saved to: %s\n",path);
	return true;
}


/*
 * gets a command from the user and parses it.
 */
SPCommand getCommandFromUser(){
	char input[SP_MAX_LINE_LENGTH];
	fgets(input,SP_MAX_LINE_LENGTH,stdin);
	return spParserPraseLine(input);
}

/*
 * prints an error for an invalid command.
 */
void printInvalidCommand(){
	printf("ERROR: invalid command\n");
}

/*
 * prints a message to the user after a success change in the game mode made by it.
 */
void printSuccessGameMode(int game_mode){
	printf("Game mode is set to ");
	if(game_mode==1)
		printf("1-player\n");
	else
		printf("2-player\n");
}

/*
 * gets a parsed command of changing the game mode setting.
 * if the command is legal, the function sets the field game_mode of 'settings'
 * to the game_mode argument from the parsed command and prints a success message to the user.
 * otherwise, prints an error to the user and returns.
 */
void executeGameMode(Settings *settings,SPCommand spc){
	if(!(spc.validArg)){
		printf("Wrong game mode\n");
		return;
	}
	setGameMode(settings,spc.arg);
	printSuccessGameMode(spc.arg);
}


/*
 * prints a message to the user after a success change in the game difficulty made by it.
 */
void printSuccessDifficulty(int diff){
	printf("Difficulty level is set to ");
	switch(diff){
		case(1):
			printf("amateur\n");
			break;
		case(2):
			printf("easy\n");
			break;
		case(3):
			printf("moderate\n");
			break;
		case(4):
			printf("hard\n");
			break;
		default:
			printf("expert\n");
			break;
	}
}

/*
 * gets a parsed command of changing the game difficulty setting.
 * if the command is legal, the function sets the field difficulty of 'settings'
 * to the difficulty argument from the parsed command and prints a success message to the user.
 * otherwise, prints an error to the user and returns.
 */
void executeDifficulty(Settings *settings,SPCommand spc){
	if(settings->game_mode==2){
		printInvalidCommand();
		return;
	}
	if(!(spc.validArg)){
		printf("Wrong difficulty level. The value should be between 1 to 5\n");
		return;
	}
	setDifficulty(settings,spc.arg);
	printSuccessDifficulty(spc.arg);
}

/*
 * prints a message to the user after a success change in the game user color made by it.
 */
void printSuccessUserColor(int user_color){
	printf("User color is set to ");
	if(user_color == 0)
		printf("black\n");
	else
		printf("white\n");
}

/*
 * gets a parsed command of changing the game user color setting.
 * if the command is legal, the function sets the field user_color of 'settings'
 * to the user_color argument from the parsed command and prints a success message to the user.
 * otherwise, prints an error to the user and returns.
 */
void executeUserColor(Settings *settings,SPCommand spc){
	if(settings->game_mode==2){
		printInvalidCommand();
		return;
	}
	if(!(spc.validArg)){
		printf("Wrong user color. The value should be 0 or 1\n");
		return;
	}
	setUserColor(settings,spc.arg);
	printSuccessUserColor(spc.arg);
}

/*
 * gets a parsed command of loading a saved game.
 * if the command is legal, the function sets the 'settings' fields according to the settings
 * of the saved game and sets the game ('src') gameboard according to the saved game gameboard.
 * otherwise, prints an error to the user and returns.
 */
void executeLoad(SPChessGame* src,Settings *settings,SPCommand spc){
	if(!(spc.validArg)){
		printInvalidCommand();
		return;
	}
	loadGame(spc.path,src,settings);
}

/*
 * gets a parsed command of changing the current game settings to their defaults.
 * The function sets 'settings' fields to their defaults and prints a success
 * message to the user.
 */
void executeDefault(Settings *settings){
	setDefault(settings);
	printf("All settings reset to default\n");
}


/*
 * gets a parsed command of printing the current game settings.
 * The function prints the 'settings' fields according to the 'settings' game_mode field.
 */
void executePrintSettings(Settings *settings){
	printSettings(settings);
}


/*
 * execute a settings mode command according to the parsed command 'spc'.
 * if the command is legal, the function changes one or more of the game settings
 * ('settings') or changes the current game ('src') gameboard, depends on the command.
 * otherwise, prints an error to the user says that the command is invalid.
 */
void executeSettingsCommand(SPChessGame* src,Settings *settings,SPCommand spc){
	switch(spc.cmd){
		case SP_GAME_MODE:
			executeGameMode(settings,spc);
			break;
		case SP_DIFFICULTY:
			executeDifficulty(settings,spc);
			break;
		case SP_USER_COLOR:
			executeUserColor(settings,spc);
			break;
		case SP_LOAD:
			executeLoad(src,settings,spc);
			break;
		case SP_DEFAULT:
			executeDefault(settings);
			break;
		case SP_PRINT_SETTINGS:
			executePrintSettings(settings);
			break;
		default:
			printInvalidCommand();
			break;
	}
}


/*
 * this function represents the settings mode.
 * reset the game board and update the settings according to the user input.
 */
SP_COMMAND settingsMode(SPChessGame* src , Settings *settings){
	createBoard(src);
	printf("Specify game settings or type 'start' to begin a game with the current settings:\n");
	SPCommand spc = getCommandFromUser();
	while(spc.cmd != SP_QUIT && spc.cmd !=SP_START){
		executeSettingsCommand(src,settings,spc);
		spc = getCommandFromUser();
	}
	return spc.cmd;
}



void freeResources(SPChessGame* src , Settings *settings){
	destroySettings(settings);
	spChessGameDestroy(src);
}


/*
 * prints a message to the user that the program is exiting and
 * frees all allocated resources.
 */
void executeQuit(SPChessGame* src , Settings *settings){
	printf("Exiting...\n");
	freeResources(src ,settings);
}


/*
 * prints a message to the user asks him to enter his next move in the game.
 * if the game_mode is set to 2-player, this message will be printed to both users.
 */
void printEnterYourMove(SPChessGame* src){
	printf("Enter your move ");
	if(src->currentPlayer==SP_CHESS_GAME_BLACK_PLAYER)
		printf("(black player):\n");
	else
		printf("(white player):\n");
}

/*
 * execution of the command "get moves"
 */
SP_CHESS_GAME_TURN_MESSAGE executeGetMoves(SPChessGame* src , SPCommand spc){
	if(!spc.validArg)
		printf("Invalid position on the board\n");
	else
		getMoves(src,spc.source);
	return RETRY;
}

/*
 * * execution of the command "save"
 */
SP_CHESS_GAME_TURN_MESSAGE executeSave(SPChessGame* src , SPCommand spc,Settings* settings){
	saveGame(spc.path , src, settings);
	return RETRY;
}


SP_CHESS_GAME_TURN_MESSAGE executeUndo(SPChessGame* src){
	SP_CHESS_GAME_UNDO_MESSAGE message = spChessGameUndoPrevMove(src);
	if(er != NONE)
		return RETRY;
	if(message==SP_CHESS_GAME_NO_HISTORY){
		if(console)
			printf("Empty history, no move to undo\n");
		return RETRY;
	}
	message = spChessGameUndoPrevMove(src);
	if(er != NONE)
		return RETRY;
	if(console)
		spChessGamePrintBoard(src);
	if(message==SP_CHESS_GAME_NO_HISTORY)
			return CHANGE_TURN;
	return RETRY;
}


SP_CHESS_GAME_TURN_MESSAGE executeMove(SPChessGame* src, SPCommand spc){
	SP_CHESS_GAME_MOVE_MESSAGE mes;
	if(!(spc.validArg))
		mes = SP_CHESS_GAME_INVALID_POSITION;
	else{
		mes = isValidMove(src, spc.source, spc.target);
		if(er != NONE)
			return CHANGE_TURN;
	}
	if(mes==SP_CHESS_GAME_SUCCESS){
		spChessGameSetMove(src,spc.source,spc.target);
		return CHANGE_TURN;
	}
	else{
		if(console)
			printMoveMessage(mes);
	}
	return RETRY;
}

/*
 * execution of a general command depends on it's type
 */
SP_CHESS_GAME_TURN_MESSAGE executeGameCommand(SPChessGame* src,SPCommand spc,Settings* settings){
	switch(spc.cmd){
		case SP_GET_MOVES:
			return executeGetMoves(src,spc);
		case SP_SAVE:
			return executeSave(src,spc,settings);
		case SP_UNDO:
			return executeUndo(src);
		case SP_MOVE:
			return executeMove(src,spc);
		default:
			if(console)
				printInvalidCommand();
			return RETRY;
	}
}



/*
 * simulates a turn of a user
 */
SP_COMMAND playUserTurn(SPChessGame* src, Settings* settings, bool toPrint){
	SP_CHESS_CHECK_WINNER_MESSAGE winnerMessage;
	SP_CHESS_GAME_TURN_MESSAGE turnMessage;
	if(toPrint && console)
		spChessGamePrintBoard(src);
	printEnterYourMove(src);
	SPCommand spc = getCommandFromUser();
	while(spc.cmd != SP_QUIT && spc.cmd !=SP_RESET){
		turnMessage = executeGameCommand(src,spc,settings);
		if(er != NONE)
			return SP_GAME_OVER;
		if(turnMessage==CHANGE_TURN)
			break;
		printEnterYourMove(src);
		spc = getCommandFromUser();
	}
	if(spc.cmd==SP_MOVE){
		winnerMessage=spChessCheckWinner(src);
		if(er != NONE)
			return SP_GAME_OVER;
		if(console)
			printWinnerMessage(winnerMessage);
		if(isGameOver(winnerMessage))
			return SP_GAME_OVER;
		return SP_MOVE;
	}
	return spc.cmd;
}


/*
 * return the name of the tool 'tool'
 */
char* toolToStr(char tool){
	if(tool=='M' || tool=='m')
		return "pawn";
	else if(tool=='N' || tool=='n')
		return "knight";
	else if(tool=='B' || tool=='b')
		return "bishop";
	else if(tool=='R' || tool=='r')
		return "rook";
	else if(tool=='Q' || tool=='q')
		return "queen";
	else //case king
		return "king";
}

/*
 * execute a computer's move
 */
void executeComputerMove(SPChessGame* src, MinimaxMove move){
	SPCell source = move.source;
	SPCell target = move.target;
	int row = getRow(source) , col = getCol(source);
	char tool = src->gameBoard[row][col] , *toolStr = toolToStr(tool);
	spChessGameSetMove(src,source,target);
	if(er != NONE)
		return;
	if(console)
		printf("Computer: move %s at <%d,%c> to <%d,%c>\n",toolStr,source.row,source.col,target.row,target.col);
}



SP_COMMAND playComputerTurn(SPChessGame* src, int difficulty){
	MinimaxMove compMove = suggestMove(src,difficulty);
	if(er != NONE)
		return SP_GAME_OVER;
	executeComputerMove(src, compMove);
	if(er != NONE)
		return SP_GAME_OVER;
	SP_CHESS_CHECK_WINNER_MESSAGE winnerMessage = spChessCheckWinner(src);
	if(er != NONE)
		return SP_GAME_OVER;
	if(console)
		printWinnerMessage(winnerMessage);
	if(isGameOver(winnerMessage))
		return SP_GAME_OVER;
	return SP_MOVE;
}

/*
 * running the game in 1-player mode (game_mode = 1)
 */
SP_COMMAND onePlayerGame(SPChessGame* src,int difficulty,int user_color, Settings* settings){
	SP_COMMAND compTurn,userTurn;
	if(user_color != (src->currentPlayer-'0')){ //case user_color is black && computer starts
		compTurn = playComputerTurn(src,difficulty);
		if(er != NONE)
			return SP_GAME_OVER;
	}
	userTurn = playUserTurn(src,settings,true);
	if(er != NONE)
		return SP_GAME_OVER;
	while(userTurn != SP_RESET && userTurn != SP_QUIT && userTurn != SP_GAME_OVER){
		compTurn = playComputerTurn(src,difficulty);
		if(er != NONE)
			return SP_GAME_OVER;
		if(compTurn==SP_GAME_OVER)
			return compTurn;
		userTurn = playUserTurn(src,settings,true);
		if(er != NONE)
			return SP_GAME_OVER;
	}
	if(userTurn==SP_RESET)
		return settingsMode(src,settings);
	return userTurn;

}


/*
 * running the game in 2-player mode (game_mode = 2)
 */
SP_COMMAND twoPlayerGame(SPChessGame* src,Settings* settings){
	SP_COMMAND turn = playUserTurn(src,settings,true);
	if(er != NONE)
		return SP_GAME_OVER;
	while(turn != SP_RESET && turn != SP_QUIT && turn != SP_GAME_OVER){
		if(turn == SP_UNDO)
			turn = playUserTurn(src,settings,false);
		else
			turn = playUserTurn(src,settings,true);
		if(er != NONE)
			return SP_GAME_OVER;
	}
	if(turn==SP_RESET)
		return settingsMode(src,settings);
	return turn;
}

/*
 * executing the 'start' command, depends on game_mode field of 'settings'
 */
SP_COMMAND executeStart(SPChessGame* src,Settings* settings){
	printf("Starting game...\n");
	if(settings->game_mode==1)
		return onePlayerGame(src,settings->difficulty,settings->user_color,settings);
	else
		return twoPlayerGame(src,settings);
}


void printError(GAME_ERROR er){
	switch(er){
		case MEM_ALLOC:
			printf("ERROR: a memory allocation error occured, terminating...\n");
			break;
		case SDL_ERROR:
			printf("ERROR: SDL error occured, terminating...\n");
			break;
		default:
			break;
	}
}



void mainConsole(){
	SPChessGame* game = spChessGameCreate(6);
	if(er!=NONE){
		printError(er);
		return;
	}
	Settings* settings = createSettings();
	if(er!=NONE){
		spChessGameDestroy(game);
		printError(er);
		return;
	}
	printConsoleMessage();
	SP_COMMAND command = settingsMode(game,settings);
	while(command==SP_START){
		command = executeStart(game,settings);
		if(er!=NONE){
			freeResources(game,settings);
			printError(er);
			return;
		}
	}
	if(command==SP_QUIT){
		executeQuit(game,settings);
		return;
	}
	freeResources(game,settings);
}



