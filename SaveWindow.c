
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "GUIAux.h"
#include "CHESSGame.h"
#include "settings.h"
#include "Button.h"
#include "MainAux.h"
#include "MainMenu.h"


/*
 * handle events in the save\load window
 */
void handleEventsSaveLoadWindow(SDL_Event* e,SP_CHESS_GAME_GUI_WINDOW* window,Button* saveWindowButtons[],
		Button* slots[],int* offset,int* selectedSlot,bool* toSaveLoad, int numSlots){

	if(e->type == SDL_QUIT){
		(*window) = QUIT_WINDOW;
		return;
	}
	handleButtonEvent(saveWindowButtons[0],e,offset,NULL); //upArrow
	handleButtonEvent(saveWindowButtons[1],e,offset,&numSlots); //downArrow
	handleButtonEvent(saveWindowButtons[2],e,window,NULL); //back
	handleButtonEvent(saveWindowButtons[3],e,window,toSaveLoad); //saveGame or loadGame

	for(int i=(*offset); i<(*offset)+3; i++)
		handleButtonEvent(slots[i],e,selectedSlot,&i);
}

/*
 * action to apply if the 'back' button clicked
 */
void actionBackSaveWindow(void* window, void* unused){
	if(unused==NULL)
		*(SP_CHESS_GAME_GUI_WINDOW*)window = QUIT_WINDOW;
}

/*
 * action to apply if the 'up arrow' button clicked
 */
void actionUpArrow(void* offset, void* unused){
	if(unused==NULL){
		if((*((int*)offset))>0)
			(*((int*)offset))--;
	}
}

/*
 * action to apply if the 'down arrow' button clicked
 */
void actionDownArrow(void* offset, void* numSlots){
	if((*((int*)offset))+3 < (*((int*)numSlots)))
			(*((int*)offset))++;
}

/*
 * action to apply if the 'save' button clicked in case of save window
 */
void actionToSave(void* window, void* toSave){
	*(SP_CHESS_GAME_GUI_WINDOW*)window = QUIT_WINDOW;
	*(bool*)toSave = true;
}

/*
 * action to apply if the 'load' button clicked in case of load window
 */
void actionToLoad(void* window, void* toLoad){
	*(SP_CHESS_GAME_GUI_WINDOW*)window = QUIT_WINDOW;
	*(bool*)toLoad = true;
}

/*
 * action to apply if one of the 'slot' buttons clicked.
 * i.e. if the user tried to load a game from a slot in which he\she saved a game in.
 */
void actionSlot(void* selectedSlot, void* index){
	*(int*)selectedSlot = *(int*)index + 1;
}

/*
 * action to apply if one of the 'off slot' buttons clicked.
 * i.e. if the user tried to load a game from a slot in which he\she didn't save a game in.
 */
void actionOffSlot(void* selectedSlot, void* index){
	int *unused1 = (int*)selectedSlot;
	int *unused2 = (int*)index;
	unused1 += 0;
	unused2 += 0;
	return;
}

/*
 * update the locations of the slots that should be presented in the save\load window,
 * according to clicks on the 'down arrow' or 'up arrow' buttons
 */
void updateSlotsLocations(Button* slots[],Button* vButton,int offset,int selectedSlot,
		SDL_Rect slotsPositions[]){

	slots[offset]->location = slotsPositions[0];
	slots[offset+1]->location = slotsPositions[1];
	slots[offset+2]->location = slotsPositions[2];
	if(offset<selectedSlot && selectedSlot<=offset+3)
		vButton->location.y = slotsPositions[selectedSlot-offset-1].y;
}

/*
 * updates the array 'slotsToShow' with 1's according to the slots that the user saved
 * previous games in. if the user saved a previous game in 'sloti' than slotsToShow[i-1]
 * will be set to 1, otherwise it will be set to 0.
 */
void updateSlotsToShow(int slotsToShow[], int numSlots){
	char name[200];
	for(int i=1; i<=numSlots; i++){
		sprintf(name, "savedGame%d.txt", i);
		if( access(name,F_OK) != -1){ //file exist
			slotsToShow[i-1] = 1;
		}
		else
			slotsToShow[i-1] = 0;
	}
}

/*
 * initialize 'slot' buttons according to the number of slots to save\load defined by 'numSlots'.
 * after the initialization of a 'slot' button, inserts it to the array 'slots'.
 */
void initializeSlotButtons(SDL_Renderer* rend, Button* slots[], int numSlots, bool isSave){

	int slotsToShow[numSlots];
	updateSlotsToShow(slotsToShow,numSlots);
	SDL_Rect slotsPos[numSlots];
	for(int i=0; i<numSlots; i++){
		SDL_Rect rect = { .x=130, .y=(50+(i*100)), .w=150, .h=50};
		slotsPos[i] = rect;
	}
	char str[200];
	for(int i=1; i<=numSlots; i++){
		if(!isSave && slotsToShow[i-1] == 0){
			sprintf(str, "pictures/slotOff%d.bmp", i);
			slots[i-1] = createButton(rend, str, slotsPos[i-1],actionOffSlot,true,0,0,0);
		}
		else{
			sprintf(str, "pictures/slot%d.bmp", i);
			slots[i-1] = createButton(rend, str, slotsPos[i-1],actionSlot,true,0,0,0);
		}

	}
}

/*
 * drawing the window's permanent buttons and the three 'slot' buttons that should be presented
 * (according to clicks on 'down arrow' & 'up arrow' buttons) to the renderer 'rend'
 */
void drawSaveLoadWindowButtons(SDL_Renderer* rend,Button* saveWindowButtons[],Button* slots[],
		int offset,int selectedSlot){

	drawButtons(rend,saveWindowButtons,4);
	if(offset<selectedSlot && selectedSlot<=offset+3)
		drawButton(saveWindowButtons[4],rend);
	drawButtonsFrom(rend,slots,offset,offset+3);
	//SDL_RenderPresent(rend);
}

/*
 * present 'Game Loaded' image to the user after a successfull load
 */
void presentGameLoaded(SDL_Renderer* rend){
	SDL_Surface* loadedGameSurface = SDL_LoadBMP("pictures/gameLoaded.bmp");
		if (loadedGameSurface == NULL) {
			er = SDL_ERROR;
			return;
	}

	SDL_SetColorKey(loadedGameSurface, SDL_TRUE, SDL_MapRGB(loadedGameSurface->format, 0, 0, 0));

	SDL_Texture* loadedGameTex = SDL_CreateTextureFromSurface(rend, loadedGameSurface);
	if (loadedGameTex == NULL) {
		SDL_FreeSurface(loadedGameSurface);
		er = SDL_ERROR;
		return;
	}

	SDL_FreeSurface(loadedGameSurface);

	SDL_Rect rect = { .x = 2, .y = 120, .w = 445, .h = 210 };

	if(SDL_RenderCopy(rend, loadedGameTex, NULL, &rect) == -1){
		er = SDL_ERROR;
		SDL_DestroyTexture(loadedGameTex);
		return;
	}

	SDL_RenderPresent(rend);
	SDL_Delay(1500);
	SDL_DestroyTexture(loadedGameTex);
}

/*
 * present 'Game Saved' image to the user after a successfull save
 */
void presentGameSaved(SDL_Renderer* rend){
	SDL_Surface* savedGameSurface = SDL_LoadBMP("pictures/gameSaved.bmp");
	if (savedGameSurface == NULL) {
		er = SDL_ERROR;
		return;
	}

	SDL_SetColorKey(savedGameSurface, SDL_TRUE, SDL_MapRGB(savedGameSurface->format, 0, 0, 0));

	SDL_Texture* savedGameTex = SDL_CreateTextureFromSurface(rend, savedGameSurface);
	if (savedGameTex == NULL) {
		er = SDL_ERROR;
		SDL_FreeSurface(savedGameSurface);
		return;
	}

	SDL_FreeSurface(savedGameSurface);

	SDL_Rect rect = { .x = 40, .y = 120, .w = 370, .h = 210 };

	if(SDL_RenderCopy(rend, savedGameTex, NULL, &rect) == -1){
		er = SDL_ERROR;
		SDL_DestroyTexture(savedGameTex);
		return;
	}

	SDL_RenderPresent(rend);
	SDL_Delay(1500);
	SDL_DestroyTexture(savedGameTex);
}

/*
 * pop a messagebox to the user after a try to save\load a game
 * without choosing a specific slot
 */
int SDLmessageBoxSlotNotChosen(){
	   const SDL_MessageBoxButtonData buttons[] = {
	        { 0, 0, "ok" }
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
	    const SDL_MessageBoxData slotMessageBox = {
	        SDL_MESSAGEBOX_INFORMATION,
	        NULL,
	        "Save/Load ERROR",
	        "Please choose a slot to perform a save/load",
	        SDL_arraysize(buttons),
	        buttons,
	        &colorScheme
	    };

	    int buttonid = 0;
		if (SDL_ShowMessageBox(&slotMessageBox, &buttonid) < 0) {
			return 1;
		}
	    return buttonid;

}


/*
 * pop a messagebox to notify the user that her try to save\load a game
 * caused an error and didn't execute
 */
int SDLmessageBoxSaveOrLoadFailed(int toLoad){
	   const SDL_MessageBoxButtonData buttons[] = {
	        { 0, 0, "close" }
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
	    const SDL_MessageBoxData saveMessageBox = {
	        SDL_MESSAGEBOX_INFORMATION,
	        NULL,
	        "Save ERROR",
	        "Save operation failure",
	        SDL_arraysize(buttons),
	        buttons,
	        &colorScheme
	    };

	    const SDL_MessageBoxData loadMessageBox = {
	        SDL_MESSAGEBOX_INFORMATION,
	        NULL,
	        "Load ERROR",
	        "Load operation failure",
	        SDL_arraysize(buttons),
	        buttons,
	        &colorScheme
	    };

	    int buttonid = 0;
	    if(toLoad){
		    if (SDL_ShowMessageBox(&loadMessageBox, &buttonid) < 0) {
		        return 1;
		    }
	    }
	    else{
		    if (SDL_ShowMessageBox(&saveMessageBox, &buttonid) < 0) {
		        return 1;
		    }
	    }
	    return buttonid;

}


int saveLoadGameWindow(SPChessGame* game, Settings* settings, int numSlots,bool isSave){
	//SDL initialization
	if (SDL_Init(SDL_INIT_VIDEO) < 0){
		er = SDL_ERROR;
		return 0;
	}

	SDL_Renderer* rend;
	SDL_Texture* tex[1];
	int numTex = 0;

	// create an SDL window
	SDL_Window* saveWindow = SDL_CreateWindow(
		"Save / Load Game",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		450,
		450,
		SDL_WINDOW_SHOWN);

	// check if the window properly created
	if (saveWindow == NULL ) {
		er = SDL_ERROR;
		SDL_Quit();
		return 0;
	}

	// create an SDL renderer and checks if the creation executed properly
	rend = SDL_CreateRenderer(saveWindow, -1, SDL_RENDERER_SOFTWARE);
	if (rend == NULL){
		er = SDL_ERROR;
		destroyGUIElements(saveWindow, rend, tex, numTex);
		SDL_Quit();
		return 0;
	}

	//enable images transparency.
	if(SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND) == -1){
		er = SDL_ERROR;
		destroyGUIElements(saveWindow, rend, tex, numTex);
		SDL_Quit();
		return 0;
	}

	// create a surf for the background image
	SDL_Surface* backgroundSurf = SDL_LoadBMP("pictures/background.bmp");
	if (backgroundSurf == NULL) {
		er = SDL_ERROR;
		destroyGUIElements(saveWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	// create a texture for the background image
	SDL_Texture* backgroundTex = SDL_CreateTextureFromSurface(rend, backgroundSurf);
	if (backgroundTex == NULL) {
		SDL_FreeSurface(backgroundSurf);
		er = SDL_ERROR;
		destroyGUIElements(saveWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_FreeSurface(backgroundSurf);
	updateTexArray(tex, &numTex, backgroundTex);

	//drawing the background image to the renderer
	if(SDL_RenderCopy(rend, backgroundTex, NULL, NULL) == -1){
		er = SDL_ERROR;
		destroyGUIElements(saveWindow, rend, tex, numTex);
		SDL_Quit();
		return 0;
	}
	SDL_RenderPresent(rend);

	//creating locations for the 'save\load window' buttons
	SDL_Rect upArrow = { .x = 310, .y = 85, .w = 65, .h = 55 };
	SDL_Rect downArrow = { .x = 310, .y = 215, .w = 65, .h = 55 };
	SDL_Rect backPos = { .x = 80, .y = 345, .w = 125, .h = 45 };
	SDL_Rect vPos = { .x = 75 , .y = 50 , .w = 65, .h = 55};
	SDL_Rect saveGamePos = { .x = 230, .y = 345, .w = 130, .h = 45};
	SDL_Rect firstSlot = { .x = 130, .y = 50, .w = 150, .h = 50};
	SDL_Rect secondSlot = { .x = 130, .y = 150, .w = 150, .h = 50};
	SDL_Rect thirdSlot = { .x = 130, .y = 250, .w = 150, .h = 50};
	SDL_Rect slotsPositions[3] = {firstSlot, secondSlot, thirdSlot};

	//creating the 'window manage' buttons of the save\load window
	Button* upArrowButton = createButton(rend,"pictures/blackPawn.bmp",upArrow,actionUpArrow, true, 0, 0, 0);
	Button* downArrowButton = createButton(rend,"pictures/blackPawnFliped.bmp",downArrow,actionDownArrow, true, 0, 0, 0);
	Button* backButton = createButton(rend,"pictures/back.bmp",backPos,actionBackSaveWindow, true, 0, 0, 0);
	Button* vButton = createButton(rend,"pictures/blackKnight.bmp",vPos,NULL, true, 0, 0, 0);
	Button* saveLoadGameButton;
	if(isSave)
		saveLoadGameButton = createButton(rend,"pictures/SaveGameSaveWindow.bmp",saveGamePos, actionToSave, true,0 ,0 ,0);
	else
		saveLoadGameButton = createButton(rend,"pictures/LoadGameSaveWindow.bmp",saveGamePos, actionToLoad, true,0 ,0 ,0);
	Button* saveLoadWindowButtons[5] = {upArrowButton,downArrowButton,backButton,saveLoadGameButton,vButton};

	Button* slots[numSlots];

	//creating the 'slot' buttons
	initializeSlotButtons(rend, slots , numSlots, isSave);

	//checking if an SDL\allocation error happend during buttons creation process
	if(er != NONE){
		er = SDL_ERROR;
		destroyButtons(saveLoadWindowButtons, 5);
		destroyButtons(slots, numSlots);
		destroyGUIElements(saveWindow, rend, tex, numTex);
		SDL_Quit();
		return 0;
	}

	SP_CHESS_GAME_GUI_WINDOW window = SAME_WINDOW;
	int offset = 0;
	int selectedSlot = 0;
	bool toSaveLoad = false ,success = false;
	SDL_Event e;

	while(window == SAME_WINDOW){

		if(SDL_RenderCopy(rend, backgroundTex, NULL, NULL) == -1){
			er = SDL_ERROR;
			destroyButtons(saveLoadWindowButtons, 5);
			destroyButtons(slots, numSlots);
			destroyGUIElements(saveWindow, rend, tex, numTex);
			SDL_Quit();
			return 0;
		}

		updateSlotsLocations(slots,vButton,offset,selectedSlot,slotsPositions);

		drawSaveLoadWindowButtons(rend,saveLoadWindowButtons,slots,offset,selectedSlot);

		SDL_RenderPresent(rend);

		if(er != NONE){
			destroyButtons(saveLoadWindowButtons, 5);
			destroyButtons(slots, numSlots);
			destroyGUIElements(saveWindow, rend, tex, numTex);
			SDL_Quit();
			return 0;
		}

		if(SDL_WaitEvent(&e) == 0){
			er = SDL_ERROR;
			destroyButtons(saveLoadWindowButtons, 5);
			destroyButtons(slots, numSlots);
			destroyGUIElements(saveWindow, rend, tex, numTex);
			SDL_Quit();
			return 0;
		}
		handleEventsSaveLoadWindow(&e,&window,saveLoadWindowButtons,slots,&offset,&selectedSlot,
				&toSaveLoad, numSlots);

		if(toSaveLoad && selectedSlot==0){
			window = SAME_WINDOW;
			SDL_HideWindow(saveWindow);
			SDLmessageBoxSlotNotChosen();
			SDL_ShowWindow(saveWindow);
			toSaveLoad = false;
		}
	}

	if(SDL_RenderCopy(rend, backgroundTex, NULL, NULL) == -1){
		er = SDL_ERROR;
		destroyButtons(saveLoadWindowButtons, 5);
		destroyButtons(slots, numSlots);
		destroyGUIElements(saveWindow, rend, tex, numTex);
		SDL_Quit();
		return 0;
	}

	SDL_RenderPresent(rend);

	//preforms the saving of the game
	if(isSave && toSaveLoad){
		char str[200];
		sprintf(str, "savedGame%d.txt", selectedSlot);
		success = saveGame(str,game,settings);
		if(success){
			presentGameSaved(rend);
			if(er != NONE){
				destroyButtons(saveLoadWindowButtons, 5);
				destroyButtons(slots, numSlots);
				destroyGUIElements(saveWindow, rend, tex, numTex);
				SDL_HideWindow(saveWindow);
				SDLmessageBoxSaveOrLoadFailed(0);
				SDL_ShowWindow(saveWindow);
				return 0;
			}
		}
		else{
			destroyButtons(saveLoadWindowButtons, 5);
			destroyButtons(slots, numSlots);
			destroyGUIElements(saveWindow, rend, tex, numTex);
			SDL_HideWindow(saveWindow);
			SDLmessageBoxSaveOrLoadFailed(0);
			SDL_ShowWindow(saveWindow);
			return 0;
		}
	}
	//preforms the loading of a game
	if( !isSave && toSaveLoad){
		char str2[200];
		sprintf(str2, "savedGame%d.txt", selectedSlot);
		success = loadGame(str2,game,settings);
		if(success){
			presentGameLoaded(rend);
			if(er != NONE){
				destroyButtons(saveLoadWindowButtons, 5);
				destroyButtons(slots, numSlots);
				destroyGUIElements(saveWindow, rend, tex, numTex);
				SDL_HideWindow(saveWindow);
				SDLmessageBoxSaveOrLoadFailed(0);
				SDL_ShowWindow(saveWindow);
				return 0;
			}
		}
		else{
			destroyButtons(saveLoadWindowButtons, 5);
			destroyButtons(slots, numSlots);
			destroyGUIElements(saveWindow, rend, tex, numTex);
			SDL_HideWindow(saveWindow);
			SDLmessageBoxSaveOrLoadFailed(0);
			SDL_ShowWindow(saveWindow);
			return 0;
		}
	}

	//destroy all window elements and return
	destroyButtons(saveLoadWindowButtons, 5);
	destroyButtons(slots, numSlots);
	destroyGUIElements(saveWindow, rend, tex, numTex);
	if(success)
		return 1;
	return 0;
}
