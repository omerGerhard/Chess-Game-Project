#include "ToolButton.h"
#include "GameWindow.h"
#include "GUIAux.h"
#include "MainAux.h"
#include "MainMenu.h"
#include "cell.h"
#include "SaveWindow.h"
#include "Button.h"
#include <ctype.h>

//settings global variable.
Settings* globSet;

/*
 * handle events in the game window
 */
void handleEventsGameWindow(SPChessGame* game, SDL_Event* e,Button* gameMenuButtons[],
		ToolButton* whiteTools[], ToolButton* blackTools[],
		SP_CHESS_GAME_GUI_WINDOW* window, int* updateLocations, int* whiteLast, int* printLast,
		SP_CHESS_GAME_TURN_MESSAGE* userTurn, int* endOfGame, GetMovesMatrix* mat){

	bool toolClicked = false;
	if(e->type == SDL_QUIT){
		*window = QUIT_WINDOW;
		return;
	}
	if(!(*endOfGame)){
		for(int i=0; i<16; i++){
			handleToolButtonEvent(whiteTools[i], e, game, updateLocations, whiteLast, printLast,
					&toolClicked,userTurn, mat);
		}
		for(int i=0; i<16; i++){
			handleToolButtonEvent(blackTools[i], e, game, updateLocations, whiteLast, printLast,
					&toolClicked,userTurn, mat);
		}
	}

	if(er != NONE)
		return;

	if(!toolClicked){
		handleButtonEvent(gameMenuButtons[0], e, game,NULL);
		handleButtonEvent(gameMenuButtons[1], e, window,NULL); //SAVE GAME
		handleButtonEvent(gameMenuButtons[2], e, window, NULL); //LOAD GAME
		handleButtonEvent(gameMenuButtons[3], e, game,userTurn);
		handleButtonEvent(gameMenuButtons[4], e, window,NULL);
		handleButtonEvent(gameMenuButtons[5], e, window,NULL);
		*updateLocations = 1;
	}

}

/*
 * using for the 'get_moves' feature
 * returning values based on the different scenarios
 * based on these values we presenting the 'get_moves'
 * relevant moves.
 */
int getMovesEncoding(int threat, int capt){
	if(threat && capt)
		return 4;
	else if(threat)
		return 3;
	else if(capt)
		return 2;
	else
		return 1;
}

/*
 * print all the valid moves for the tool in 'srcMat->cell' cell:
 * draw blue square if the move is just a valid move.
 * draw light blue square if the move captures an opponent tool.
 * draw red square if the move causing the tool to be threatened.
 * draw yellow square if the move causing the tool both to be threaten and capture opponent tool.
 */
void getMovesGUI(void* src,void* srcMat){
	SPChessGame* game = (SPChessGame*) src;
	GetMovesMatrix* mat = (GetMovesMatrix*) srcMat;
	SPCell source = mat->cell, target;
	int threat , capt, sourceRow = getRow(source), sourceCol = getCol(source);
	char tool = game->gameBoard[sourceRow][sourceCol];
	char player = toolToPlayer(tool);
	setToZero(mat->matrix);
	validMoves(game,source,tool,mat->matrix);
	if(er != NONE)
		return;
	for(int i=0; i<SP_CHESS_GAME_N_ROWS; i++){
		for(int j=0; j<SP_CHESS_GAME_N_COLUMNS; j++){
			if(mat->matrix[i][j]!=0){
				target = spCellCreate(i+1,j+'A');
				threat = isThreatend(game, source, target, player);
				if(er != NONE)
					return;
				capt = mat->matrix[i][j]==2 ? 1 : 0;
				mat->matrix[i][j] = getMovesEncoding(threat, capt);
			}
		}
	}
}

/*
 * action to apply when 'Undo' button clicked.
 */
void actionUndo(void *src, void* userTurnSrc){
	SPChessGame* game = (SPChessGame*)src;
	SP_CHESS_GAME_TURN_MESSAGE* userTurn = (SP_CHESS_GAME_TURN_MESSAGE*)userTurnSrc;
	SP_CHESS_CHECK_WINNER_MESSAGE winnerMessage = spChessCheckWinner(game);
	if(isGameOver(winnerMessage) && globSet->game_mode == 1){
		if((globSet->user_color==0 && game->currentPlayer=='1') ||
				(globSet->user_color==1 && game->currentPlayer=='0') ){
			spChessGameUndoPrevMove(game);
			if(er != NONE)
				return;
			*(userTurn) = RETRY;
		}
	}
	else
		*(userTurn) = executeUndo(game);
}

/*
 * action to apply when 'Restart' button clicked.
 */
void actionRestart(void *game, void* unused){
	if(unused == NULL)
		createBoard((SPChessGame*)game);
}

/*
 * action to apply when 'Main Menu' button clicked.
 */
void actionMainMenu(void *window, void* unused){
	if(unused == NULL){
		*(SP_CHESS_GAME_GUI_WINDOW*) window = MAIN_MENU_WINDOW;
	}
}

/*
 * action to apply when 'Save Game' button clicked.
 */
void actionSaveGame(void* window, void* unused){
	if(unused == NULL)
		*(SP_CHESS_GAME_GUI_WINDOW*) window = SAVE_WINDOW;
}

SPCell createCellFromCoords(int x, int y){
	int r = (y/CHESS_BOARD_CELL_SIZE);
	int c = (x/CHESS_BOARD_CELL_SIZE);
	return spCellCreate(8 - r, 'A'+c);
}

/*
 * create array of buttons that represent tools.
 */
void createToolArr(ToolButton** toolArr, int numTool,SDL_Renderer* rend, const char* image,
		void (*action)(void*,void*),bool transp, int r, int g, int b, int color, int baseIndex){
	for(int i=0; i<numTool; i++)
		toolArr[i] = createToolButton(rend, image, action, transp, r, g, b,color,baseIndex+i);
}

/*
 * create array of buttons that represent the player tools.
 */
void createPlayerToolArr(ToolButton** toolsArr,ToolButton** pawns,ToolButton** bishops,
		ToolButton** knights,ToolButton** rooks,ToolButton* queen,ToolButton* king){
	for(int i=0; i<8; i++)
		toolsArr[i] = pawns[i];
	toolsArr[8] = bishops[0];
	toolsArr[9] = bishops[1];
	toolsArr[10] = knights[0];
	toolsArr[11] = knights[1];
	toolsArr[12] = rooks[0];
	toolsArr[13] = rooks[1];
	toolsArr[14] = queen;
	toolsArr[15] = king;
}

/*
 * set both the white and black tools to be not presented on the board.
 */
void setOffToPresent(ToolButton** whiteTools,ToolButton** blackTools){
	for(int i=0; i<16; i++){
		whiteTools[i]->toPresent = false;
		blackTools[i]->toPresent = false;
	}
}

/*
 * update white tool location on the board and set it to be presented.
 */
void updateWhiteToolPresentation(char tool,int *whitePawnsInd,int *whiteBishopsInd,
		int *whiteKnightsInd,int *whiteRooksInd,ToolButton** whiteTools,SDL_Rect location){
	if(tool == 'm'){
		whiteTools[*whitePawnsInd]->basedLocation = location;
		whiteTools[*whitePawnsInd]->location = location;
		whiteTools[*whitePawnsInd]->toPresent = true;
		(*whitePawnsInd)++;
	}
	else if(tool == 'b'){
		whiteTools[*whiteBishopsInd]->basedLocation = location;
		whiteTools[*whiteBishopsInd]->location = location;
		whiteTools[*whiteBishopsInd]->toPresent = true;
		(*whiteBishopsInd)++;
	}
	else if(tool == 'n'){
		whiteTools[*whiteKnightsInd]->basedLocation = location;
		whiteTools[*whiteKnightsInd]->location = location;
		whiteTools[*whiteKnightsInd]->toPresent = true;
		(*whiteKnightsInd)++;
	}
	else if(tool == 'r'){
		whiteTools[*whiteRooksInd]->basedLocation = location;
		whiteTools[*whiteRooksInd]->location = location;
		whiteTools[*whiteRooksInd]->toPresent = true;
		(*whiteRooksInd)++;
	}
	else if(tool == 'q'){
		whiteTools[14]->basedLocation = location;
		whiteTools[14]->location = location;
		whiteTools[14]->toPresent = true;
	}
	else{
		whiteTools[15]->basedLocation = location;
		whiteTools[15]->location = location;
		whiteTools[15]->toPresent = true;
	}
}

/*
 * update black tool location on the board and set it to be presented.
 */
void updateBlackToolPresentation(char tool,int *blackPawnsInd,int *blackBishopsInd,int *blackKnightsInd,
		int *blackRooksInd,ToolButton** blackTools,SDL_Rect location){
	if(tool == 'M'){
		blackTools[*blackPawnsInd]->basedLocation = location;
		blackTools[*blackPawnsInd]->location = location;
		blackTools[*blackPawnsInd]->toPresent = true;
		(*blackPawnsInd)++;
	}
	else if(tool == 'B'){
		blackTools[*blackBishopsInd]->basedLocation = location;
		blackTools[*blackBishopsInd]->location = location;
		blackTools[*blackBishopsInd]->toPresent = true;
		(*blackBishopsInd)++;
	}
	else if(tool == 'N'){
		blackTools[*blackKnightsInd]->basedLocation = location;
		blackTools[*blackKnightsInd]->location = location;
		blackTools[*blackKnightsInd]->toPresent = true;
		(*blackKnightsInd)++;
	}
	else if(tool == 'R'){
		blackTools[*blackRooksInd]->basedLocation = location;
		blackTools[*blackRooksInd]->location = location;
		blackTools[*blackRooksInd]->toPresent = true;
		(*blackRooksInd)++;
	}
	else if(tool == 'Q'){
		blackTools[14]->basedLocation = location;
		blackTools[14]->location = location;
		blackTools[14]->toPresent = true;
	}
	else{
		blackTools[15]->basedLocation = location;
		blackTools[15]->location = location;
		blackTools[15]->toPresent = true;
	}
}

/*
 * based on the game board of 'game'
 * update the tools locations and set the relevant tools to be presented.
 */
void setToolsLocations(SPChessGame* game,ToolButton** whiteTools,ToolButton** blackTools,SDL_Rect boardPos[8][8]){
	setOffToPresent(whiteTools,blackTools);
	int whitePawnsInd = 0, whiteBishopsInd = 8, whiteKnightsInd = 10, whiteRooksInd = 12;
	int blackPawnsInd = 0, blackBishopsInd = 8, blackKnightsInd = 10, blackRooksInd = 12;
	char tool;
	for(int i=0; i<8; i++){
		for(int j=0; j<8; j++){
			tool = game->gameBoard[i][j];
			if(tool != '_'){
				if(islower(tool)){
					updateWhiteToolPresentation(tool,&whitePawnsInd,&whiteBishopsInd,&whiteKnightsInd,
							&whiteRooksInd,whiteTools,boardPos[i][j]);
				}
				else{
					updateBlackToolPresentation(tool,&blackPawnsInd,&blackBishopsInd,&blackKnightsInd,
							&blackRooksInd,blackTools,boardPos[i][j]);
				}
			}
		}
	}
}

/*
 * return true if there is a tool (black or white) which is
 * being moved on the board, otherwise return false.
 */
bool isMoving(ToolButton** whiteTools,ToolButton** blackTools){
	for(int i=0; i<16; i++){
		if(whiteTools[i]->move || blackTools[i]->move)
			return true;
	}
	return false;
}

/*
 * presenting the game window.
 */
void presentGameWindow(SPChessGame* game,SDL_Renderer* rend, SDL_Texture* backgroundTex,SDL_Texture* chessBoardTex,SDL_Rect gameBoard,
		SDL_Texture* cellMarks[],Button** gameMenuButtons,
		ToolButton** whiteTools,ToolButton** blackTools,
		int whiteLast,int printLast,SP_CHESS_CHECK_WINNER_MESSAGE winnerMessage, SDL_Rect boardPos[8][8], GetMovesMatrix* mat){

	//drawing the background image.
	if(SDL_RenderCopy(rend, backgroundTex, NULL, NULL) == -1){
		er = SDL_ERROR;
		return;
	}

	//drawing the board image.
	if(SDL_RenderCopy(rend, chessBoardTex, NULL, &gameBoard) == -1){
		er = SDL_ERROR;
		return;
	}
	//drawing the game menu buttons.
	drawButtons(rend, gameMenuButtons, GAME_MENU_BUTTONS);

	//drawing the tools buttons.
	if(whiteLast){
		drawToolButtons(rend, blackTools, NUM_PLAYER_CHESS_TOOLS,printLast);
		drawToolButtons(rend, whiteTools, NUM_PLAYER_CHESS_TOOLS,printLast);
	}
	else{
		drawToolButtons(rend, whiteTools, NUM_PLAYER_CHESS_TOOLS,printLast);
		drawToolButtons(rend, blackTools, NUM_PLAYER_CHESS_TOOLS,printLast);
	}

	//checking if the game is over and react accordingly.
	if(isGameOver(winnerMessage)){
		if(winnerMessage == CHECKMATE_WHITE_PLAYER){
			SDL_Rect finito = { .x=0, .y=240, .w=640, .h=100};
			if(SDL_RenderCopy(rend, cellMarks[1], NULL, &finito) == -1){
				er = SDL_ERROR;
				return;
			}
		}
		else if(winnerMessage == CHECKMATE_BLACK_PLAYER){
			SDL_Rect finito = { .x=0, .y=240, .w=640, .h=100};
			if(SDL_RenderCopy(rend, cellMarks[2], NULL, &finito) == -1){
				er = SDL_ERROR;
				return;
			}
		}
		else{
			SDL_Rect finito = { .x=170, .y=240, .w=300, .h=100};
			if(SDL_RenderCopy(rend, cellMarks[3], NULL, &finito) == -1){
				er = SDL_ERROR;
				return;
			}
		}
	}
	//perform the 'get_moves'.
	else if(mat->show){
		SDL_Rect pos;
		for(int i=0; i<8; i++){
			for(int j=0; j<8; j++){
				if(mat->matrix[i][j]){
					pos = boardPos[i][j];
					if(SDL_RenderCopy(rend, cellMarks[3 + mat->matrix[i][j]], NULL, &pos) == -1){
						er = SDL_ERROR;
						return;
					}
				}
			}
		}
	}
	//drawing the check(when the king is threatened) square.
	else{
		bool motion = isMoving(whiteTools, blackTools);
		if(!motion){
			if(game->isBlackKingThreatened)
				if(SDL_RenderCopy(rend, cellMarks[0], NULL, &(blackTools[15]->location)) == -1){
					er = SDL_ERROR;
					return;
				}
			if(game->isWhiteKingThreatened)
				if(SDL_RenderCopy(rend, cellMarks[0], NULL, &(whiteTools[15]->location)) == -1){
					er = SDL_ERROR;
					return;
				}
		}
	}
	//presenting the window.
	SDL_RenderPresent(rend);
}

/*
 * pop a message box to the user to ask her
 * if she wants to save the current game before leaving the window.
 */
int SDLmessageBoxAskToSave(){
	   const SDL_MessageBoxButtonData buttons[] = {
		        { 0, 0, "no" },
		        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "yes" },
		        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "cancel" },
	    };
	    const SDL_MessageBoxColorScheme colorScheme = {
	        {
	            /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
	            { 255,   0,   0 },
	            /* [SDL_MESSAGEBOX_COLOR_TEXT] */
	            {   0, 255,   0 },
	            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
	            { 255, 255,   0 },
	            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
	            {   0,   0, 255 },
	            /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
	            { 255,   0, 255 }
	        }
	    };
	    const SDL_MessageBoxData saveMessageBox = {
	        SDL_MESSAGEBOX_INFORMATION,
	        NULL,
	        "To Save",
	        "Current game is not saved. Do you want to save your game?",
	        SDL_arraysize(buttons),
	        buttons,
	        &colorScheme
	    };

	    int buttonid = 0;
		if (SDL_ShowMessageBox(&saveMessageBox, &buttonid) < 0) {
			return 2;
		}
		return buttonid;
}


/*
 * playing a turn of a user.
 */
SP_CHESS_GAME_TURN_MESSAGE playUserTurnGUI(SPChessGame* game,SDL_Event* e,Button* gameMenuButtons[],
		ToolButton** whiteTools, ToolButton** blackTools, SP_CHESS_GAME_GUI_WINDOW* window,
		int* updateLocations, int* whiteLast, int* printLast,int* endOfGame, GetMovesMatrix* mat){

	SP_CHESS_GAME_TURN_MESSAGE userTurn = RETRY;
	if(SDL_WaitEvent(e) == 0){
		er = SDL_ERROR;
		return CHANGE_TURN;
	}
	handleEventsGameWindow(game, e, gameMenuButtons, whiteTools, blackTools,
			window, updateLocations, whiteLast, printLast, &userTurn, endOfGame, mat);
	return userTurn;
}

/*
 * perform the user turn.
 */
SP_CHESS_GAME_GUI_WINDOW userTurnGUI(SP_CHESS_GAME_TURN_MESSAGE userTurn, SPChessGame* game,
		Settings* settings, SDL_Event* e,Button* gameMenuButtons[],ToolButton** whiteTools,
		ToolButton** blackTools, int* updateLocations, int* whiteLast, int* printLast,
		SDL_Rect boardPos[8][8], SDL_Renderer* rend, SDL_Texture* backgroundTex, SDL_Texture* chessBoardTex, SDL_Rect gameBoard,
		SDL_Texture* cellMarks[],int* endOfGame, GetMovesMatrix* mat, bool* currentGameIsSaved,
		SDL_Window* gameWindow)
{
	SP_CHESS_GAME_GUI_WINDOW window = SAME_WINDOW;
	int saved;
	SP_CHESS_CHECK_WINNER_MESSAGE winnerMessage;
	while(userTurn == RETRY){
		if(*updateLocations){
			setToolsLocations(game,whiteTools,blackTools,boardPos);
			*updateLocations = 0;
		}
		winnerMessage = spChessCheckWinner(game);
		if(er != NONE)
			return QUIT_WINDOW;
		(*endOfGame) = isGameOver(winnerMessage);
		presentGameWindow(game,rend, backgroundTex,chessBoardTex,gameBoard,cellMarks,gameMenuButtons,
				whiteTools,blackTools,*whiteLast,*printLast,winnerMessage, boardPos, mat);
		if(er != NONE)
			return QUIT_WINDOW;
		userTurn = playUserTurnGUI(game,e,gameMenuButtons,whiteTools,blackTools,&window,updateLocations,
				whiteLast, printLast,endOfGame, mat);
		if(er != NONE)
			return QUIT_WINDOW;
		if(window == SAVE_WINDOW){
			SDL_HideWindow(gameWindow);
			saved = saveLoadGameWindow(game,settings,NUM_OF_SLOTS,true);
			SDL_ShowWindow(gameWindow);
			if(er != NONE)
				return QUIT_WINDOW;
			if(saved == 1) //game was saved
				*(currentGameIsSaved) = true;
			window = SAME_WINDOW;
		}
		if(window == LOAD_WINDOW){
			SDL_HideWindow(gameWindow);
			saveLoadGameWindow(game,settings,NUM_OF_SLOTS,false);
			SDL_ShowWindow(gameWindow);
			if(er != NONE)
				return QUIT_WINDOW;
			window = SAME_WINDOW;
		}
		if(window == QUIT_WINDOW || window == MAIN_MENU_WINDOW){
			if(!(*currentGameIsSaved)){ //if the game is not saved before this click
				SDL_HideWindow(gameWindow);
				int buttonPressed = SDLmessageBoxAskToSave();
				SDL_ShowWindow(gameWindow);
				if(buttonPressed == 1){ //user chose 'yes'
					SDL_HideWindow(gameWindow);
					saved = saveLoadGameWindow(game,settings,NUM_OF_SLOTS,true);
					SDL_ShowWindow(gameWindow);
					if(er != NONE)
						return QUIT_WINDOW;
					if(saved == 0) //game was not saved
						window = SAME_WINDOW;
				}
				else if(buttonPressed == 2) //user chose 'cancel'
					window = SAME_WINDOW;
			}
		}
		if(window != SAME_WINDOW)
			break;
	}

	winnerMessage = spChessCheckWinner(game);
	if(er != NONE)
		return QUIT_WINDOW;
	(*endOfGame) = isGameOver(winnerMessage);

	*(currentGameIsSaved) = false;
	setToolsLocations(game,whiteTools,blackTools,boardPos);
	return window;
}

SP_CHESS_GAME_GUI_WINDOW gameMenuWindow(SPChessGame* game, Settings* settings){
	if (SDL_Init(SDL_INIT_VIDEO) < 0){
		er = SDL_ERROR;
		return QUIT_WINDOW;
	}
	globSet = settings;
	SDL_Renderer* rend = NULL;
	SDL_Texture* tex[10];
	int numTex = 0;

	// create an SDL window
	SDL_Window* gameWindow = SDL_CreateWindow(
			"Chess Game",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			800,
			640,
			SDL_WINDOW_SHOWN);

	if (gameWindow == NULL ) {
		er = SDL_ERROR;
		SDL_Quit();
		return QUIT_WINDOW;
	}

	//create renderer for the window.
	rend = SDL_CreateRenderer(gameWindow, -1, SDL_RENDERER_SOFTWARE);
	if (rend == NULL){
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	//enable images transparency.
	if(SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND) == -1){
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	//create surface for background image.
	SDL_Surface* backgroundSurf = SDL_LoadBMP("pictures/background.bmp");
	if (backgroundSurf == NULL) {
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	//create texture for background image.
	SDL_Texture* backgroundTex = SDL_CreateTextureFromSurface(rend, backgroundSurf);
	if (backgroundTex == NULL) {
		SDL_FreeSurface(backgroundSurf);
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_FreeSurface(backgroundSurf);
	updateTexArray(tex, &numTex, backgroundTex);

	SDL_Rect gameBoard = { .x = 0, .y = 0, .w = 640, .h = 640 };

	//create surface for board image.
	SDL_Surface* chessboardSurf = SDL_LoadBMP("pictures/ChessBoard.bmp");
	if (chessboardSurf == NULL) {
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	//create texture for board image.
	SDL_Texture* chessboardTex = SDL_CreateTextureFromSurface(rend, chessboardSurf);
	if (chessboardTex == NULL) {
		SDL_FreeSurface(chessboardSurf);
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_FreeSurface(chessboardSurf);
	updateTexArray(tex, &numTex, chessboardTex);

	//create surface for checked square image.
	SDL_Surface* kingCheckedSurf = SDL_LoadBMP("pictures/checkedKing.bmp");
	if (kingCheckedSurf == NULL) {
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_SetColorKey(kingCheckedSurf, SDL_TRUE, SDL_MapRGB(kingCheckedSurf->format, 255, 0, 0));

	//create texture for checked square image.
	SDL_Texture* kingCheckedTex = SDL_CreateTextureFromSurface(rend, kingCheckedSurf);
	if (kingCheckedTex == NULL) {
		SDL_FreeSurface(kingCheckedSurf);
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_FreeSurface(kingCheckedSurf);
	updateTexArray(tex, &numTex, kingCheckedTex);

	//create surface for white checkmate message image.
	SDL_Surface* checkmateWhitePlayerSurf = SDL_LoadBMP("pictures/winnerWhite.bmp");
	if (checkmateWhitePlayerSurf == NULL) {
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_SetColorKey(checkmateWhitePlayerSurf, SDL_TRUE, SDL_MapRGB(checkmateWhitePlayerSurf->format, 0, 0, 0));

	//create texture for white checkmate message image.
	SDL_Texture* checkmateWhitePlayerTex = SDL_CreateTextureFromSurface(rend, checkmateWhitePlayerSurf);
	if (checkmateWhitePlayerTex == NULL) {
		SDL_FreeSurface(checkmateWhitePlayerSurf);
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_FreeSurface(checkmateWhitePlayerSurf);
	updateTexArray(tex, &numTex, checkmateWhitePlayerTex);

	//create surface for white checkmate message image.
	SDL_Surface* checkmateBlackPlayerSurf = SDL_LoadBMP("pictures/winnerBlack.bmp");
	if (checkmateBlackPlayerSurf == NULL) {
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_SetColorKey(checkmateBlackPlayerSurf, SDL_TRUE, SDL_MapRGB(checkmateBlackPlayerSurf->format, 0, 0, 0));

	//create texture for white checkmate message image.
	SDL_Texture* checkmateBlackPlayerTex = SDL_CreateTextureFromSurface(rend, checkmateBlackPlayerSurf);
	if (checkmateBlackPlayerTex == NULL) {
		SDL_FreeSurface(checkmateBlackPlayerSurf);
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_FreeSurface(checkmateBlackPlayerSurf);
	updateTexArray(tex, &numTex, checkmateBlackPlayerTex);

	//create surface for draw message image.
	SDL_Surface* drawSurf = SDL_LoadBMP("pictures/winnerDraw.bmp");
	if (drawSurf == NULL) {
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_SetColorKey(drawSurf, SDL_TRUE, SDL_MapRGB(drawSurf->format, 0, 0, 0));

	//create texture for draw message image.
	SDL_Texture* drawTex = SDL_CreateTextureFromSurface(rend, drawSurf);
	if (drawTex == NULL) {
		SDL_FreeSurface(drawSurf);
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_FreeSurface(drawSurf);
	updateTexArray(tex, &numTex, drawTex);

	//create surface for valid move square image.
	SDL_Surface* moveSurf = SDL_LoadBMP("pictures/squareStandart.bmp");
	if (moveSurf == NULL) {
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_SetColorKey(moveSurf, SDL_TRUE, SDL_MapRGB(moveSurf->format, 255, 0, 0));

	//create texture for valid move square image.
	SDL_Texture* moveTex = SDL_CreateTextureFromSurface(rend, moveSurf);
	if (moveTex == NULL) {
		SDL_FreeSurface(moveSurf);
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_FreeSurface(moveSurf);
	updateTexArray(tex, &numTex, moveTex);

	//create surface for move capture square image.
	SDL_Surface* captSurf = SDL_LoadBMP("pictures/squareCapture.bmp");
	if (captSurf == NULL) {
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_SetColorKey(captSurf, SDL_TRUE, SDL_MapRGB(captSurf->format, 255, 0, 0));

	//create texture for move capture square image.
	SDL_Texture* captTex = SDL_CreateTextureFromSurface(rend, captSurf);
	if (captTex == NULL) {
		SDL_FreeSurface(captSurf);
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_FreeSurface(captSurf);
	updateTexArray(tex, &numTex, captTex);

	//create surface for move threat square image.
	SDL_Surface* threatSurf = SDL_LoadBMP("pictures/squareThreatened.bmp");
	if (threatSurf == NULL) {
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_SetColorKey(threatSurf, SDL_TRUE, SDL_MapRGB(threatSurf->format, 255, 0, 0));

	//create texture for move threat square image.
	SDL_Texture* threatTex = SDL_CreateTextureFromSurface(rend, threatSurf);
	if (threatTex == NULL) {
		SDL_FreeSurface(threatSurf);
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_FreeSurface(threatSurf);
	updateTexArray(tex, &numTex, threatTex);

	//create surface for move capt & threat square image.
	SDL_Surface* bothSurf = SDL_LoadBMP("pictures/squareBoth.bmp");
	if (bothSurf == NULL) {
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_SetColorKey(bothSurf, SDL_TRUE, SDL_MapRGB(bothSurf->format, 255, 0, 0));

	//create texture for move capt & threat square image.
	SDL_Texture* bothTex = SDL_CreateTextureFromSurface(rend, bothSurf);
	if (bothTex == NULL) {
		SDL_FreeSurface(bothSurf);
		er = SDL_ERROR;
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_FreeSurface(bothSurf);
	updateTexArray(tex, &numTex, bothTex);

	SDL_Texture* cellMarks[] = {kingCheckedTex, checkmateWhitePlayerTex, checkmateBlackPlayerTex,
			drawTex, moveTex, captTex, threatTex, bothTex};

	SDL_Rect restartGamePos = { .x = 655, .y = 100, .w = 130, .h = 30 };
	SDL_Rect saveGamePos = { .x = 640, .y = 170, .w = 160, .h = 30 };
	SDL_Rect loadGamePos = { .x = 640, .y = 240, .w = 160, .h = 30 };
	SDL_Rect undoMovePos = { .x = 675, .y = 310, .w = 80, .h = 30 };
	SDL_Rect mainMenuPos = { .x = 640, .y = 380, .w = 160, .h = 30 };
	SDL_Rect quitPos = { .x = 677, .y = 450, .w = 75, .h = 30 };

	//create game menu buttons.
	Button* restartGame = createButton(rend,"pictures/restartButton.bmp",restartGamePos,actionRestart, true, 0, 0, 0);
	Button* saveGame = createButton(rend,"pictures/saveGame.bmp",saveGamePos,actionSaveGame, true, 0, 0, 0);
	Button* loadGame = createButton(rend,"pictures/loadGameWindow.bmp",loadGamePos,actionLoadGame, true, 0, 0, 0);
	Button* undoMove = createButton(rend,"pictures/undo.bmp",undoMovePos,actionUndo, true, 0, 0, 0);
	Button* mainMenu = createButton(rend,"pictures/mainMenu.bmp",mainMenuPos,actionMainMenu, true, 0, 0, 0);
	Button* quit = createButton(rend,"pictures/quitGame.bmp",quitPos,actionQuit, true, 0, 0, 0);

	Button* gameMenuButtons[] = {restartGame, saveGame, loadGame, undoMove, mainMenu, quit};
	//drawButtons(rend, gameMenuButtons, GAME_MENU_BUTTONS);

	if(er != NONE){
		destroyButtons(gameMenuButtons,GAME_MENU_BUTTONS);
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_Rect boardPos[8][8];
	for(int row=0; row<8; row++){
		for(int col=0; col<8; col++){
			boardPos[row][col].x = col*CHESS_BOARD_CELL_SIZE;
			boardPos[row][col].y = (7-row)*CHESS_BOARD_CELL_SIZE;
			boardPos[row][col].h = boardPos[row][col].w = CHESS_BOARD_CELL_SIZE;
		}
	}

	//create game tools.
	ToolButton* whitePawns[8];
	ToolButton* blackPawns[8];
	ToolButton* whiteBishops[2];
	ToolButton* blackBishops[2];
	ToolButton* whiteKnights[2];
	ToolButton* blackKnights[2];
	ToolButton* whiteRooks[2];
	ToolButton* blackRooks[2];
	ToolButton* whiteQueen = createToolButton(rend, WHITE_QUEEN_IMAGE, getMovesGUI, true, 0, 0, 0, 1, 14);
	ToolButton* blackQueen = createToolButton(rend, BLACK_QUEEN_IMAGE, getMovesGUI, true, 0, 0, 0, 0, 14);
	ToolButton* whiteKing = createToolButton(rend, WHITE_KING_IMAGE, getMovesGUI, true, 0, 0, 0, 1, 15);
	ToolButton* blackKing = createToolButton(rend, BLACK_KING_IMAGE, getMovesGUI, true, 0, 0, 0, 0, 15);

	createToolArr(whitePawns, 8, rend, WHITE_PAWN_IMAGE, getMovesGUI, true, 0, 0, 0, 1, 0);
	createToolArr(blackPawns, 8, rend, BLACK_PAWN_IMAGE, getMovesGUI, true, 0, 0, 0, 0, 0);
	createToolArr(whiteBishops, 2, rend, WHITE_BISHOP_IMAGE, getMovesGUI, true, 0, 0, 0, 1, 8);
	createToolArr(blackBishops, 2, rend, BLACK_BISHOP_IMAGE, getMovesGUI, true, 0, 0, 0, 0, 8);
	createToolArr(whiteKnights, 2, rend, WHITE_KNIGHT_IMAGE, getMovesGUI, true, 0, 0, 0, 1, 10);
	createToolArr(blackKnights, 2, rend, BLACK_KNIGHT_IMAGE, getMovesGUI, true, 0, 0, 0, 0, 10);
	createToolArr(whiteRooks, 2, rend, WHITE_ROOK_IMAGE, getMovesGUI, true, 0, 0, 0, 1, 12);
	createToolArr(blackRooks, 2, rend, BLACK_ROOK_IMAGE, getMovesGUI, true, 0, 0, 0, 0, 12);

	ToolButton* whiteTools[NUM_PLAYER_CHESS_TOOLS];
	ToolButton* blackTools[NUM_PLAYER_CHESS_TOOLS];
	createPlayerToolArr(whiteTools,whitePawns,whiteBishops,whiteKnights,whiteRooks,whiteQueen,whiteKing);
	createPlayerToolArr(blackTools,blackPawns,blackBishops,blackKnights,blackRooks,blackQueen,blackKing);

	if(er != NONE){
		destroyToolButtons(whiteTools,NUM_PLAYER_CHESS_TOOLS);
		destroyToolButtons(blackTools,NUM_PLAYER_CHESS_TOOLS);
		destroyButtons(gameMenuButtons,GAME_MENU_BUTTONS);
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	GetMovesMatrix mat;
	mat.show = false;
	mat.matrix = allocateNewMatrix(8, 8);

	if(er != NONE){
		destroyToolButtons(whiteTools,NUM_PLAYER_CHESS_TOOLS);
		destroyToolButtons(blackTools,NUM_PLAYER_CHESS_TOOLS);
		destroyButtons(gameMenuButtons,GAME_MENU_BUTTONS);
		destroyGUIElements(gameWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_Event e;
	int printLast=0 , whiteLast=0 , updateLocations=1, endOfGame=0;
	SP_CHESS_GAME_GUI_WINDOW window = SAME_WINDOW;
	SP_CHESS_GAME_TURN_MESSAGE userTurn = RETRY;
	bool currentGameIsSaved = false;

	if(settings->user_color==1 && game->currentPlayer=='1'){
		window = userTurnGUI(userTurn, game, settings,&e,gameMenuButtons,whiteTools,blackTools,&updateLocations,
				&whiteLast, &printLast, boardPos, rend, backgroundTex, chessboardTex, gameBoard,
				cellMarks,&endOfGame, &mat, &currentGameIsSaved, gameWindow);
		if(er != NONE){
			destroyToolButtons(whiteTools,NUM_PLAYER_CHESS_TOOLS);
			destroyToolButtons(blackTools,NUM_PLAYER_CHESS_TOOLS);
			destroyButtons(gameMenuButtons,GAME_MENU_BUTTONS);
			destroyGUIElements(gameWindow, rend, tex, numTex);
			destroyMatrix(mat.matrix,8);
			SDL_Quit();
			return QUIT_WINDOW;
		}
	}

	while(window == SAME_WINDOW){
		if(settings->game_mode == 1 && !endOfGame){
			presentGameWindow(game,rend, backgroundTex,chessboardTex,gameBoard,cellMarks,gameMenuButtons,whiteTools,
					blackTools,whiteLast,printLast,ALL_GOOD, boardPos, &mat);
			if(er != NONE){
				destroyToolButtons(whiteTools,NUM_PLAYER_CHESS_TOOLS);
				destroyToolButtons(blackTools,NUM_PLAYER_CHESS_TOOLS);
				destroyButtons(gameMenuButtons,GAME_MENU_BUTTONS);
				destroyGUIElements(gameWindow, rend, tex, numTex);
				destroyMatrix(mat.matrix,8);
				SDL_Quit();
				return QUIT_WINDOW;
			}
			playComputerTurn(game,settings->difficulty);
			if(er != NONE){
				destroyToolButtons(whiteTools,NUM_PLAYER_CHESS_TOOLS);
				destroyToolButtons(blackTools,NUM_PLAYER_CHESS_TOOLS);
				destroyButtons(gameMenuButtons,GAME_MENU_BUTTONS);
				destroyGUIElements(gameWindow, rend, tex, numTex);
				destroyMatrix(mat.matrix,8);
				SDL_Quit();
				return QUIT_WINDOW;
			}
			setToolsLocations(game,whiteTools,blackTools,boardPos);
		}
		userTurn = RETRY;
		window = userTurnGUI(userTurn, game, settings, &e,gameMenuButtons,whiteTools,blackTools,&updateLocations,
				&whiteLast, &printLast, boardPos, rend, backgroundTex, chessboardTex, gameBoard, cellMarks,&endOfGame,
				&mat, &currentGameIsSaved, gameWindow);
		if(er != NONE){
			destroyToolButtons(whiteTools,NUM_PLAYER_CHESS_TOOLS);
			destroyToolButtons(blackTools,NUM_PLAYER_CHESS_TOOLS);
			destroyButtons(gameMenuButtons,GAME_MENU_BUTTONS);
			destroyGUIElements(gameWindow, rend, tex, numTex);
			destroyMatrix(mat.matrix,8);
			SDL_Quit();
			return QUIT_WINDOW;
		}
	}

	destroyToolButtons(whiteTools,NUM_PLAYER_CHESS_TOOLS);
	destroyToolButtons(blackTools,NUM_PLAYER_CHESS_TOOLS);
	destroyButtons(gameMenuButtons,GAME_MENU_BUTTONS);
	destroyGUIElements(gameWindow, rend, tex, numTex);
	destroyMatrix(mat.matrix,8);
	SDL_Quit();

	createBoard(game);
	return window;

}

