
#include <stdio.h>
#include <stdlib.h>
#include "Button.h"
#include "GUIAux.h"
#include "MainAux.h"
#include "MainMenu.h"
#include "SettingsMenuWindow.h"
#include "GameWindow.h"

/*
 * switching between windows - opens 'window' Window
 */
SP_CHESS_GAME_GUI_WINDOW windowManager(SP_CHESS_GAME_GUI_WINDOW window, SPChessGame* game, Settings* settings){
	switch(window){
		case MAIN_MENU_WINDOW:
			return mainMenuWindow(game, settings);
		case SETTINGS_MENU_WINDOW:
			return settingsMenuWindow(settings);
		case GAME_WINDOW:
			return gameMenuWindow(game, settings);
		default:
			return QUIT_WINDOW;
	}
}


void updateTexArray(SDL_Texture* tex[], int* texNum, SDL_Texture* texture){
	tex[(*texNum)] = texture;
	(*texNum)++;
}


void destroyGUIElements(SDL_Window* window, SDL_Renderer* rend, SDL_Texture* tex[], int texNum){
	for(int i=0; i<texNum; i++)
		SDL_DestroyTexture(tex[i]);
	if(rend != NULL)
		SDL_DestroyRenderer(rend);
	if(window != NULL)
		SDL_DestroyWindow(window);
}

/*
 * pop a messagebox to notify the user that an SDL error occured
 */
void SDLmessageBoxGUIError(){
	   const SDL_MessageBoxButtonData buttons[] = {
		        { 0, 0, "close" },
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
	    const char* str;
	    if(er == SDL_ERROR)
	    	str = "SDL error occured, terminating the program.";
	    else
	    	str = "Memory error occured, terminating the program.";
	    const SDL_MessageBoxData errorMessageBox = {
	    	 SDL_MESSAGEBOX_INFORMATION,
	         NULL,
	         "GUI ERROR",
	         str,
	         SDL_arraysize(buttons),
	         buttons,
	         &colorScheme
	   	 };

	    int buttonid = 0;
		SDL_ShowMessageBox(&errorMessageBox, &buttonid);
}

/*
 * print to console that a GUI error occured and that the program is terminating.
 * more over, pop a messagebox to notify the user that an SDL error occured.
 */
void printGUIError(){
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return;
	SDLmessageBoxGUIError();
	SDL_Quit();
}


void mainGUI(){
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

	SP_CHESS_GAME_GUI_WINDOW window = mainMenuWindow(game, settings);
	if(er!=NONE){
		freeResources(game,settings);
		printError(er);
		printGUIError();
		return;
	}
	while(window != QUIT_WINDOW){
		window = windowManager(window, game, settings);
		if(er!=NONE){
			freeResources(game,settings);
			printError(er);
			printGUIError();
			return;
		}
	}
	freeResources(game,settings);
}
