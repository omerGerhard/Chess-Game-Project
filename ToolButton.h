
#ifndef TOOLBUTTON_H_
#define TOOLBUTTON_H_


#include <SDL.h>
#include <SDL_video.h>
#include <stdbool.h>
#include "MainAux.h"

#define LEFT_MOUSE_CLICK 1
#define RIGHT_MOUSE_CLICK 3

typedef struct tool_button_t ToolButton;

struct tool_button_t {
	SDL_Renderer* render;
	SDL_Texture* texture;
	SDL_Rect location;
	SDL_Rect basedLocation;
	int xClickDiff;
	int yClickDiff;
	bool move;
	int color;
	int index;
	void (*action)(void *, void*);//need to be changed.
	bool toPresent;
};

typedef struct tool_get_moves_mat GetMovesMatrix;
struct tool_get_moves_mat {
	bool show;
	int** matrix;
	SPCell cell;
};

/*
 * create a new tool button. this is the button that represents the chess tools in the game.
 */
ToolButton* createToolButton(SDL_Renderer* renderer,const char* image,void (*action)(void*, void*), bool transp, int r, int g, int b
		,int color, int index);


/*
 * draw the tool button 'button' to the renderer 'rend'
 */
void drawToolButton(ToolButton* button, SDL_Renderer* rend);

/*
 * draw the tool buttons 'buttons' to the renderer 'rend'.
 * 'printLast' is the index of the last Tool button to draw to 'rend',
 * this is the tool that currently on move by the user.
 */
void drawToolButtons(SDL_Renderer* rend,ToolButton* buttons[],int numButtons,int printLast);

/*
 * free all alocated memory connected to the tool button 'button'
 */
void destroyToolButton(ToolButton* button);

/*
 * free all alocated memory connected to each tool button in the tool buttons array 'buttons'
 */
void destroyToolButtons(ToolButton* buttons[],int numButtons);

/*
 * handle tool button events
 */
void handleToolButtonEvent(ToolButton* button, SDL_Event* e, SPChessGame* game,
		int* updateLocations, int* whiteLast, int* printLast, bool* toolClicked,
		SP_CHESS_GAME_TURN_MESSAGE* userTurn, GetMovesMatrix* mat);

#endif /* TOOLBUTTON_H_ */
