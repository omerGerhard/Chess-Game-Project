

#include "Button.h"
#include "GUIAux.h"
#include "MainAux.h"

Button* createButton(SDL_Renderer* renderer,const char* image,SDL_Rect location,void (*action)(void*,void*), bool transp, int r, int g, int b){

	Button* data = (Button*) malloc(sizeof(Button));
	if (data == NULL) {
		er = MEM_ALLOC;
		return NULL;
	}

	// we use the surface as a temp var
	SDL_Surface* surface = SDL_LoadBMP(image);
	if (surface == NULL) {
		er = SDL_ERROR;
		free(data);
		return NULL;
	}

	if(transp)
		SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, r, g, b));

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == NULL) {
		er = SDL_ERROR;
		free(data);
		SDL_FreeSurface(surface);
		return NULL ;
	}

	// surface is not needed after texture is created
	SDL_FreeSurface(surface);

	// store button & widget details
	data->texture = texture;
	data->location = location;
	data->action = action;
	return data;
}


void handleButtonEvent(Button* button, SDL_Event* e, void* arg1, void* arg2)
{
	if (e->type == SDL_MOUSEBUTTONUP) {
		SDL_Point point = { .x = e->button.x, .y = e->button.y };

		if (SDL_PointInRect(&point, &button->location)) {
			button->action(arg1, arg2);
		}
	}
}


void drawButton(Button* button, SDL_Renderer* render)
{
	if(SDL_RenderCopy(render, button->texture, NULL, &button->location) == -1)
		er = SDL_ERROR;
}


void drawButtons(SDL_Renderer* rend,Button* buttons[],int numButtons){
	for(int i=0; i<numButtons; i++)
		drawButton(buttons[i],rend);
}


void drawButtonsFrom(SDL_Renderer* rend,Button* buttons[],int from,int numButtons){
	for(int i=from; i<numButtons; i++)
		drawButton(buttons[i],rend);
}


void destroyButton(Button* button)
{
	SDL_DestroyTexture(button->texture);
	free(button);
}


void destroyButtons(Button* buttons[],int numButtons){
	for(int i=0; i<numButtons; i++){
		if(buttons[i] != NULL)
			destroyButton(buttons[i]);
	}

}
