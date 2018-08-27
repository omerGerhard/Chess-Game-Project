
#ifndef SAVEWINDOW_H_
#define SAVEWINDOW_H_

#include <SDL.h>
#include <SDL_video.h>

/*
 * opening the save\load window for the user to save\load game.
 * return 1 after a succesfull save\load made by the user, otherwise 0.
 */
int saveLoadGameWindow(SPChessGame* game, Settings* settings, int numSlots, bool isSave);


#endif /* SAVEWINDOW_H_ */
