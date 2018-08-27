
#ifndef MAINMENU_H_
#define MAINMENU_H_

#include <SDL.h>
#include <SDL_video.h>
#include "MainAux.h"
#include "GUIAux.h"

#define NUM_MAIN_MENU_BUTTONS 3

/*
 * action to apply when 'Load Game' button clicked.
 */
void actionLoadGame(void* action,void* unused);
/*
 * action to apply when 'Quit Game' button clicked.
 */
void actionQuit(void* action, void* unused);

/*
 * main menu window.
 */
SP_CHESS_GAME_GUI_WINDOW mainMenuWindow(SPChessGame* game, Settings* settings);

#endif /* MAINMENU_H_ */
