
#ifndef BUTTON_H_
#define BUTTON_H_


#include <SDL.h>
#include <SDL_video.h>
#include <stdbool.h>


typedef struct button_t Button;
struct button_t {
	SDL_Renderer* render;
	SDL_Texture* texture;
	SDL_Rect location;
	void (*action)(void *,void *);
};

/*
 * create a new button.
 */
Button* createButton(SDL_Renderer* renderer,const char* image,SDL_Rect location,void (*action)(void*,void*),
		bool transp, int r, int g, int b);

/*
 * free all alocated memory connected to the button 'button'
 */
void destroyButton(Button* button);

/*
 * handle button events
 */
void handleButtonEvent(Button* button, SDL_Event* event, void* arg1,void* arg2);

/*
 * draw the button 'button' to the renderer 'rend'
 */
void drawButton(Button* button, SDL_Renderer* rend);

/*
 * draw the buttons 'buttons' to the renderer 'rend'
 */
void drawButtons(SDL_Renderer* rend,Button* buttons[],int numButtons);

/*
 * draw the buttons 'buttons' to renderer 'rend', starting from index 'from' in 'buttons'
 */
void drawButtonsFrom(SDL_Renderer* rend,Button* buttons[],int from,int numButtons);

/*
 * free all alocated memory connected to each button in the button array 'buttons'
 */
void destroyButtons(Button* buttons[],int numButtons);

#endif /* BUTTON_H_ */
