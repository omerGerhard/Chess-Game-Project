

#ifndef GUIAUX_H_
#define GUIAUX_H_


#include <SDL.h>
#include <SDL_video.h>

typedef enum sp_chess_game_gui_window_t {
	SAME_WINDOW,
	MAIN_MENU_WINDOW,
	SETTINGS_MENU_WINDOW,
	LOAD_WINDOW,
	SAVE_WINDOW,
	GAME_WINDOW,
	QUIT_WINDOW
} SP_CHESS_GAME_GUI_WINDOW;

/*
 * keep tracking on the textures created in a window in order to free their memory when
 * closing the window.
 */
void updateTexArray(SDL_Texture* tex[], int* texNum, SDL_Texture* texture);

/*
 * free all alocated memory associated with SDL Window 'window'
 */
void destroyGUIElements(SDL_Window* window, SDL_Renderer* rend, SDL_Texture* tex[], int texNum);

/*
 * starting the game in GUI mode
 */
void mainGUI();

#endif /* GUIAUX_H_ */
