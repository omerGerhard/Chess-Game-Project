
#include "MainMenu.h"
#include "MainAux.h"
#include "SaveWindow.h"
#include "GameWindow.h"
#include "Button.h"

/*
 * handle events in the main menu window
 */
void handleEventsMainWindow(SDL_Event* e, SP_CHESS_GAME_GUI_WINDOW* window,
		Button* buttons[],int numButtons){

	if(e->type == SDL_QUIT){
		*window = QUIT_WINDOW;
		return;
	}
	for(int i=0; i<numButtons; i++)
		handleButtonEvent(buttons[i], e, window,NULL);
}


/*
 * action to apply when 'New Game' button clicked.
 */
void actionNewGame(void* action,void* unused){
	if(unused == NULL)
		*(SP_CHESS_GAME_GUI_WINDOW*)action = SETTINGS_MENU_WINDOW;
}

/*
 * action to apply when 'Load Game' button clicked.
 */
void actionLoadGame(void* action,void* unused){
	if(unused == NULL)
		*(SP_CHESS_GAME_GUI_WINDOW*)action = LOAD_WINDOW;
}

/*
 * action to apply when 'Quit Game' button clicked.
 */
void actionQuit(void* action,void* unused){
	if(unused == NULL)
		*(SP_CHESS_GAME_GUI_WINDOW*)action = QUIT_WINDOW;
}

SP_CHESS_GAME_GUI_WINDOW mainMenuWindow(SPChessGame* game, Settings* settings){
	if (SDL_Init(SDL_INIT_VIDEO) < 0){
		er = SDL_ERROR;
		return QUIT_WINDOW;
	}

	SDL_Renderer* rend = NULL;
	SDL_Texture* tex[1];
	int numTex = 0;

	// create an SDL window
	SDL_Window* mainWindow = SDL_CreateWindow(
		"Chess Game - Main Menu",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		800,
		600,
		SDL_WINDOW_SHOWN);

	if (mainWindow == NULL ) {
		er = SDL_ERROR;
		SDL_Quit();
		return QUIT_WINDOW;
	}

	//create renderer for the window.
	rend = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_SOFTWARE);
	if (rend == NULL) {
		er = SDL_ERROR;
		destroyGUIElements(mainWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	//enable images transparency.
	if(SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND) == -1){
		er = SDL_ERROR;
		destroyGUIElements(mainWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	//create surface for background image.
	SDL_Surface* backgroundSurf = SDL_LoadBMP("pictures/background.bmp");
	if (backgroundSurf == NULL) {
		er = SDL_ERROR;
		destroyGUIElements(mainWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	//create texture for background image.
	SDL_Texture* backgroundTex = SDL_CreateTextureFromSurface(rend, backgroundSurf);
	if (backgroundTex == NULL) {
		SDL_FreeSurface(backgroundSurf);
		er = SDL_ERROR;
		destroyGUIElements(mainWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_FreeSurface(backgroundSurf);
	updateTexArray(tex, &numTex, backgroundTex);

	SDL_Rect newGamePos = { .x = 50, .y = 50, .w = 415, .h = 70 };
	SDL_Rect loadGamePos = { .x = 50, .y = 180, .w = 415, .h = 70 };
	SDL_Rect quitGamePos = { .x = 50, .y = 310, .w = 415, .h = 70 };

	//create window buttons.
	Button* newGame = createButton(rend,"pictures/New_Game_Button.bmp",newGamePos,actionNewGame, true, 0, 0, 0);
	Button* loadGame = createButton(rend,"pictures/Load_Game_Button.bmp",loadGamePos,actionLoadGame, true, 0, 0, 0);
	Button* quitGame = createButton(rend,"pictures/Quit_Game_Button.bmp",quitGamePos,actionQuit, true, 0, 0, 0);
	Button* mainWindowButtons[] = {newGame,loadGame,quitGame};

	if(er != NONE){
		destroyButtons(mainWindowButtons,NUM_MAIN_MENU_BUTTONS);
		destroyGUIElements(mainWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SP_CHESS_GAME_GUI_WINDOW window = SAME_WINDOW;
	SDL_Event e;

	while(window == SAME_WINDOW){
		//drawing the background image.
		if(SDL_RenderCopy(rend, backgroundTex, NULL, NULL) == -1){
			er = SDL_ERROR;
			destroyButtons(mainWindowButtons,NUM_MAIN_MENU_BUTTONS);
			destroyGUIElements(mainWindow, rend, tex, numTex);
			SDL_Quit();
			return QUIT_WINDOW;
		}

		//drawing window buttons.
		drawButtons(rend, mainWindowButtons, NUM_MAIN_MENU_BUTTONS);
		if(er != NONE){
			destroyButtons(mainWindowButtons,NUM_MAIN_MENU_BUTTONS);
			destroyGUIElements(mainWindow, rend, tex, numTex);
			SDL_Quit();
			return QUIT_WINDOW;
		}

		//presenting the window.
		SDL_RenderPresent(rend);

		//wait for events.
		if(SDL_WaitEvent(&e)==0){
			er = SDL_ERROR;
			destroyButtons(mainWindowButtons,NUM_MAIN_MENU_BUTTONS);
			destroyGUIElements(mainWindow, rend, tex, numTex);
			SDL_Quit();
			return QUIT_WINDOW;
		}
		//handling the events.
		handleEventsMainWindow(&e,&window,mainWindowButtons, NUM_MAIN_MENU_BUTTONS);
		if(window == LOAD_WINDOW){
			SDL_HideWindow(mainWindow);
			saveLoadGameWindow(game,settings,NUM_OF_SLOTS,false);
			SDL_ShowWindow(mainWindow);
			window = SAME_WINDOW;
		}
	}

	destroyButtons(mainWindowButtons,NUM_MAIN_MENU_BUTTONS);
	destroyGUIElements(mainWindow, rend, tex, numTex);
	SDL_Quit();

	return window;
}
