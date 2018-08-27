#include "ToolButton.h"
#include "GUIAux.h"
#include "GameWindow.h"



ToolButton* createToolButton(SDL_Renderer* renderer,const char* image,void (*action)(void*, void*), bool transp, int r, int g, int b
		,int color, int index){

	ToolButton* button = (ToolButton*) malloc(sizeof(ToolButton));
	if (button == NULL) {
		er = MEM_ALLOC;
		return NULL;
	}

	// we use the surface as a temp var
	SDL_Surface* surface = SDL_LoadBMP(image);
	if (surface == NULL) {
		er = SDL_ERROR;
		free(button);
		return NULL;
	}

	if(transp)
		SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, r, g, b));

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == NULL) {
		er = SDL_ERROR;
		free(button);
		SDL_FreeSurface(surface);
		return NULL ;
	}

	// surface is not needed after texture is created
	SDL_FreeSurface(surface);

	// store button & widget details
	button->texture = texture;
	button->action = action;
	button->toPresent = false;
	button->move = false;
	button->color = color;
	button->index = index;
	return button;
}


void handleToolButtonEvent(ToolButton* button, SDL_Event* e, SPChessGame* game,
		int* updateLocations, int* whiteLast, int* printLast, bool* toolClicked,
		SP_CHESS_GAME_TURN_MESSAGE* userTurn, GetMovesMatrix* mat){
	if(button->toPresent){
		if(e->type == SDL_MOUSEBUTTONDOWN){
			SDL_Point point = { .x = e->button.x, .y = e->button.y };
			if (SDL_PointInRect(&point, &button->location)) {
				*toolClicked = 1;
				if (e->button.button == SDL_BUTTON_LEFT){
					mat->show = false;
					button->xClickDiff = e->button.x - button->location.x;
					button->yClickDiff = e->button.y - button->location.y;
					button->move = true;
				}
				else if(e->button.button == SDL_BUTTON_RIGHT){
					mat->show = true;
					SPCell source = createCellFromCoords(button->basedLocation.x, button->basedLocation.y);
					mat->cell = source;
					button->action(game, mat);
				}
			}
		}
		else if(e->type == SDL_MOUSEMOTION){
			if(button->move){
				*toolClicked = 1;
				(*whiteLast) = button->color;
				(*printLast) = button->index;
				button->location.x = e->motion.x - button->xClickDiff;
				button->location.y = e->motion.y - button->yClickDiff;
			}
		}
		else if (e->type == SDL_MOUSEBUTTONUP) {
			mat->show = false;
			SDL_Point point = { .x = e->button.x, .y = e->button.y };
			if (SDL_PointInRect(&point, &button->location) && button->move){
				(*toolClicked) = 1;
				button->move = false;
				if(point.x > 640 || point.x < 0 || point.y > 640 || point.y < 0)
					button->location = button->basedLocation;
				else{
					SPCell source = createCellFromCoords(button->basedLocation.x, button->basedLocation.y);
					SPCell target = createCellFromCoords(point.x, point.y);
					SPCommand spc = createSPCommand(SP_MOVE, true, 0, source, target, "");
					SP_CHESS_GAME_TURN_MESSAGE mes = executeMove(game, spc);
					(*userTurn) = mes;
					if(mes == CHANGE_TURN){
						//button->location = button->basedLocation = boardPos[getRow(target)][getCol(target)];
						(*updateLocations) = 1;
					}
					else
						button->location = button->basedLocation;
				}
			}
		}
	}
}


void drawToolButton(ToolButton* button, SDL_Renderer* render){
	if(button->toPresent){
		if(SDL_RenderCopy(render, button->texture, NULL, &button->location) == -1)
			er = SDL_ERROR;
	}
}


void drawToolButtons(SDL_Renderer* rend,ToolButton* buttons[],int numButtons,int printLast){
	for(int i=0; i<numButtons; i++){
		if(i != printLast)
			drawToolButton(buttons[i],rend);
	}
	drawToolButton(buttons[printLast],rend);
}


void destroyToolButton(ToolButton* button){
	SDL_DestroyTexture(button->texture);
	free(button);
}


void destroyToolButtons(ToolButton* buttons[],int numButtons){
	for(int i=0; i<numButtons; i++){
		if(buttons[i] != NULL)
			destroyToolButton(buttons[i]);
	}
}
