

#ifndef SETTINGSMENUWINDOW_H_
#define SETTINGSMENUWINDOW_H_

#include <SDL.h>
#include <SDL_video.h>
#include "MainAux.h"

#define SETTINGS_MENU_GAME_MODE_BUTTONS 4
#define SETTINGS_MENU_DIFFICULTY_BUTTONS 7
#define SETTINGS_MENU_USER_COLOR_BUTTONS 4
#define SETTINGS_MENU_MANAGE_BUTTONS 2

/*
 * opening the 'settings window' for the user to choose the game settings
 */
SP_CHESS_GAME_GUI_WINDOW settingsMenuWindow(Settings* settings);


#endif /* SETTINGSMENUWINDOW_H_ */
