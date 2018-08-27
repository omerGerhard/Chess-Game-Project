

#include "GUIAux.h"
#include "SettingsMenuWindow.h"
#include "MainMenu.h"
#include "Button.h"

/*
 * determine the horizontal location of the 'V' symbol according to the chosen game mode setting
 */
int getGameModeVX(Settings* settings){
	if(settings->game_mode==1)
		return 435;
	else
		return 635;
}

/*
 * determine the vertical location of the 'V' symbol according to the chosen difficulty setting
 */
int getDifficultyVY(Settings* settings){
	if(settings->difficulty==1)
		return 175;
	else if(settings->difficulty==2)
		return 235;
	else if(settings->difficulty==3)
		return 295;
	else if(settings->difficulty==4)
		return 355;
	else
		return 415;
}

/*
 * determine the horizontal location of the 'V' symbol according
 * to the chosen user color setting.
 */
int getUserColorVX(Settings* settings){
	if(settings->user_color==1)
		return 405;
	else
		return 570;
}

/*
 * hanldle events in the settings menu window.
 */
void handleEventsSettingsMenuWindow(SDL_Event* e,SP_CHESS_GAME_GUI_WINDOW* action,Settings* settings,
		Button* settingsWindowGameModeButtons[], int numModeButtons,
		Button* settingsWindowDifficultyButtons[], int numDiffButtons,
		Button* settingsWindowUserColorButtons[], int numUserColorButtons,
		Button* settingsWindowManageButtons[], int numWindowManageButtons)
{
	Button* butt;
	if(e->type == SDL_QUIT){
		*action = QUIT_WINDOW;
		return;
	}
	for(int i=1; i<numModeButtons-1; i++){
		butt = settingsWindowGameModeButtons[numModeButtons-1];
		handleButtonEvent(settingsWindowGameModeButtons[i], e, &(butt->location),settings);
	}
	for(int i=0; i<numWindowManageButtons; i++){
		handleButtonEvent(settingsWindowManageButtons[i], e, action,NULL);
	}
	if(settings->game_mode==1){
		for(int i=1; i<numDiffButtons-1; i++){
			butt = settingsWindowDifficultyButtons[numDiffButtons-1];
			handleButtonEvent(settingsWindowDifficultyButtons[i], e, &(butt->location),settings);
		}
		for(int i=1; i<numUserColorButtons-1; i++){
			butt = settingsWindowUserColorButtons[numUserColorButtons-1];
			handleButtonEvent(settingsWindowUserColorButtons[i], e, &(butt->location),settings);
		}
	}
}

/*
 * action to apply when the 'start game' button clicked
 */
void actionStartGame(void* window,void* unused){
	if(unused == NULL)
		*(SP_CHESS_GAME_GUI_WINDOW*)window = GAME_WINDOW;
}

/*
 * action to apply when the 'back' button clicked
 */
void actionBack(void* window,void* unused){
	if(unused == NULL)
		*(SP_CHESS_GAME_GUI_WINDOW*)window = MAIN_MENU_WINDOW;
}

/*
 * action to apply when the 'one player' game mode button clicked
 */
void actionGameModeOnePlayer(void* location,void* settings){
	Settings* set = (Settings*)settings;
	set->game_mode = 1;
	(*((SDL_Rect *)location)).x = getGameModeVX(set);
}

/*
 * action to apply when the 'two player' game mode button clicked
 */
void actionGameModeTwoPlayer(void* location,void* settings){
	Settings* set = (Settings*)settings;
	set->game_mode = 2;
	(*((SDL_Rect *)location)).x = getGameModeVX(set);
}

/*
 * action to apply when the 'amateur' difficulty button clicked
 */
void actionDifficultyAmateur(void* location,void* settings){
	Settings* set = (Settings*)settings;
	set->difficulty = 1;
	(*((SDL_Rect *)location)).y = getDifficultyVY(set);
}

/*
 * action to apply when the 'easy' difficulty button clicked
 */
void actionDifficultyEasy(void* location,void* settings){
	Settings* set = (Settings*)settings;
	set->difficulty = 2;
	(*((SDL_Rect *)location)).y = getDifficultyVY(set);
}

/*
 * action to apply when the 'moderate' difficulty button clicked
 */
void actionDifficultyModerate(void* location,void* settings){
	Settings* set = (Settings*)settings;
	set->difficulty = 3;
	(*((SDL_Rect *)location)).y = getDifficultyVY(set);
}

/*
 * action to apply when the 'hard' difficulty button clicked
 */
void actionDifficultyHard(void* location,void* settings){
	Settings* set = (Settings*)settings;
	set->difficulty = 4;
	(*((SDL_Rect *)location)).y = getDifficultyVY(set);
}

/*
 * action to apply when the 'expert' difficulty button clicked
 */
void actionDifficultyExpert(void* location,void* settings){
	Settings* set = (Settings*)settings;
	set->difficulty = 5;
	(*((SDL_Rect *)location)).y = getDifficultyVY(set);
}

/*
 * action to apply when the 'white' user color button clicked
 */
void actionUserColorWhite(void* location,void* settings){
	Settings* set = (Settings*)settings;
	set->user_color = 1;
	(*((SDL_Rect *)location)).x = getUserColorVX(set);
}

/*
 * action to apply when the 'black' user color button clicked
 */
void actionUserColorBlack(void* location,void* settings){
	Settings* set = (Settings*)settings;
	set->user_color = 0;
	(*((SDL_Rect *)location)).x = getUserColorVX(set);
}


SP_CHESS_GAME_GUI_WINDOW settingsMenuWindow(Settings* settings){

	int gameModeVX , difficultyVY, userColorVX;

	//SDL initialization
	if (SDL_Init(SDL_INIT_VIDEO) < 0){
		er = SDL_ERROR;
		return QUIT_WINDOW;
	}

	SDL_Renderer* rend = NULL;
	SDL_Texture* tex[1];
	int numTex = 0;

	// create an SDL window
	SDL_Window* settingsWindow = SDL_CreateWindow(
		"Chess Game Settings",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		800,
		675,
		SDL_WINDOW_SHOWN);

	//checking window allocation
	if (settingsWindow == NULL ) {
		er = SDL_ERROR;
		SDL_Quit();
		return QUIT_WINDOW;
	}

	rend = SDL_CreateRenderer(settingsWindow, -1, SDL_RENDERER_SOFTWARE);
	if (rend == NULL) {
		er = SDL_ERROR;
		destroyGUIElements(settingsWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	if(SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND) == -1){
		er = SDL_ERROR;
		destroyGUIElements(settingsWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	//creating a surface for the background image
	SDL_Surface* backgroundSurf = SDL_LoadBMP("pictures/background.bmp");
	if (backgroundSurf == NULL) {
		er = SDL_ERROR;
		destroyGUIElements(settingsWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	//creating a texture for the background image
	SDL_Texture* backgroundTex = SDL_CreateTextureFromSurface(rend, backgroundSurf);
	if (backgroundTex == NULL) {
		SDL_FreeSurface(backgroundSurf);
		er = SDL_ERROR;
		destroyGUIElements(settingsWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_FreeSurface(backgroundSurf);
	updateTexArray(tex, &numTex, backgroundTex);

	//creating locations for the 'game mode' buttons
	SDL_Rect posGameMode = { .x = 25, .y = 75, .w = 250, .h = 50 };
	SDL_Rect posGameModeOnePlayer = { .x = 300, .y = 85, .w = 135, .h = 30 };
	SDL_Rect posGameModeTwoPlayer = { .x = 500, .y = 85, .w = 135, .h = 30 };
	gameModeVX = getGameModeVX(settings);
	SDL_Rect posGameModeV = { .x = gameModeVX, .y = 75, .w = 65, .h = 55 };

	//creating the 'game mode' buttons for the settings menu
	Button* gameMode = createButton(rend,"pictures/gameMode.bmp",posGameMode,NULL, true, 0, 0, 0);
	Button* onePlayerMode = createButton(rend,"pictures/gameMode-1-player.bmp",posGameModeOnePlayer,actionGameModeOnePlayer, true, 0, 0, 0);
	Button* twoPlayerMode = createButton(rend,"pictures/gameMode-2-player.bmp",posGameModeTwoPlayer,actionGameModeTwoPlayer, true, 0, 0, 0);
	Button* gameModeV = createButton(rend,"pictures/blackKnight.bmp",posGameModeV,NULL, true, 0, 0, 0);
	Button* settingsWindowGameModeButtons[4] = {gameMode,onePlayerMode,twoPlayerMode,gameModeV};

	//creating locations for the 'difficulty' buttons
	SDL_Rect posDifficulty = { .x = 25, .y = 175, .w = 275, .h = 45 };
	SDL_Rect posAmateur = { .x = 300, .y = 185, .w = 135, .h = 30 };
	SDL_Rect posEasy = { .x = 300, .y = 245, .w = 80, .h = 30 };
	SDL_Rect posModerate = { .x = 300, .y = 305, .w = 150, .h = 30 };
	SDL_Rect posHard = { .x = 300, .y = 365, .w = 80, .h = 30 };
	SDL_Rect posExpert = { .x = 300, .y = 425, .w = 115, .h = 30 };
	difficultyVY = getDifficultyVY(settings);
	SDL_Rect posDifficultyV = { .x = 440, .y = difficultyVY, .w = 65, .h = 55 };

	//creating the 'difficulty' buttons for the settings menu
	Button* difficulty = createButton(rend,"pictures/Difficulty.bmp",posDifficulty,NULL, true, 0, 0, 0);
	Button* amateur = createButton(rend,"pictures/amateur.bmp",posAmateur,actionDifficultyAmateur, true, 0, 0, 0);
	Button* easy = createButton(rend,"pictures/easy.bmp",posEasy,actionDifficultyEasy, true, 0, 0, 0);
	Button* moderate = createButton(rend,"pictures/moderate.bmp",posModerate,actionDifficultyModerate, true, 0, 0, 0);
	Button* hard = createButton(rend,"pictures/hard.bmp",posHard,actionDifficultyHard, true, 0, 0, 0);
	Button* expert = createButton(rend,"pictures/expert.bmp",posExpert,actionDifficultyExpert, true, 0, 0, 0);
	Button* difficultyV = createButton(rend,"pictures/blackKnight.bmp",posDifficultyV,NULL, true, 0, 0, 0);
	Button* settingsWindowDifficultyButtons[7] = {difficulty,amateur,easy,moderate,hard,expert,difficultyV};

	//creating locations for the 'user color' buttons
	SDL_Rect posUserColor = { .x = 25, .y = 500, .w = 285, .h = 45 };
	SDL_Rect posWhite = { .x = 310, .y = 510, .w = 95, .h = 30 };
	SDL_Rect posBlack = { .x = 475, .y = 510, .w = 95, .h = 30 };
	userColorVX = getUserColorVX(settings);
	SDL_Rect posUserColorV = { .x = userColorVX, .y = 500, .w = 65, .h = 55 };

	//creating the 'user color' buttons for the settings menu
	Button* userColor = createButton(rend,"pictures/userColor.bmp",posUserColor,NULL, true, 0, 0, 0);
	Button* white = createButton(rend,"pictures/whitePlayer.bmp",posWhite,actionUserColorWhite, true, 0, 0, 0);
	Button* black = createButton(rend,"pictures/blackPlayer.bmp",posBlack,actionUserColorBlack, true, 0, 0, 0);
	Button* userColorV = createButton(rend,"pictures/blackKnight.bmp",posUserColorV,NULL, true, 0, 0, 0);
	Button* settingsWindowUserColorButtons[4] = {userColor,white,black,userColorV};

	//creating locations for the 'window manage' buttons
	SDL_Rect posStart = { .x = 600, .y = 250, .w = 150, .h = 45 };
	SDL_Rect posBack = { .x = 600, .y = 350, .w = 125, .h = 45 };

	//creating the 'window manage' buttons for the settings menu
	Button* start = createButton(rend,"pictures/start-button.bmp",posStart,actionStartGame, true, 0, 0, 0);
	Button* back = createButton(rend,"pictures/back.bmp",posBack,actionBack, true, 0, 0, 0);
	Button* settingsWindowManageButtons[2] = {back,start};

	//checking if an SDL\allocation error happend during buttons creation process
	if(er != NONE){
		destroyButtons(settingsWindowGameModeButtons,SETTINGS_MENU_GAME_MODE_BUTTONS);
		destroyButtons(settingsWindowDifficultyButtons,SETTINGS_MENU_DIFFICULTY_BUTTONS);
		destroyButtons(settingsWindowUserColorButtons,SETTINGS_MENU_USER_COLOR_BUTTONS);
		destroyButtons(settingsWindowManageButtons,SETTINGS_MENU_MANAGE_BUTTONS);
		destroyGUIElements(settingsWindow, rend, tex, numTex);
		SDL_Quit();
		return QUIT_WINDOW;
	}

	SDL_Event e;
	SP_CHESS_GAME_GUI_WINDOW window = SAME_WINDOW;

	while(window == SAME_WINDOW){
		if(SDL_RenderCopy(rend, backgroundTex, NULL, NULL) == -1){
			er = SDL_ERROR;
			destroyButtons(settingsWindowGameModeButtons,SETTINGS_MENU_GAME_MODE_BUTTONS);
			destroyButtons(settingsWindowDifficultyButtons,SETTINGS_MENU_DIFFICULTY_BUTTONS);
			destroyButtons(settingsWindowUserColorButtons,SETTINGS_MENU_USER_COLOR_BUTTONS);
			destroyButtons(settingsWindowManageButtons,SETTINGS_MENU_MANAGE_BUTTONS);
			destroyGUIElements(settingsWindow, rend, tex, numTex);
			SDL_Quit();
			return QUIT_WINDOW;
		}

		drawButtons(rend, settingsWindowGameModeButtons, 4);
		drawButtons(rend, settingsWindowManageButtons, 2);
		if(er != NONE){
			destroyButtons(settingsWindowGameModeButtons,SETTINGS_MENU_GAME_MODE_BUTTONS);
			destroyButtons(settingsWindowDifficultyButtons,SETTINGS_MENU_DIFFICULTY_BUTTONS);
			destroyButtons(settingsWindowUserColorButtons,SETTINGS_MENU_USER_COLOR_BUTTONS);
			destroyButtons(settingsWindowManageButtons,SETTINGS_MENU_MANAGE_BUTTONS);
			destroyGUIElements(settingsWindow, rend, tex, numTex);
			SDL_Quit();
			return QUIT_WINDOW;
		}
		if(settings->game_mode==1){
			drawButtons(rend, settingsWindowDifficultyButtons, 7);
			drawButtons(rend, settingsWindowUserColorButtons, 4);
			if(er != NONE){
				destroyButtons(settingsWindowGameModeButtons,SETTINGS_MENU_GAME_MODE_BUTTONS);
				destroyButtons(settingsWindowDifficultyButtons,SETTINGS_MENU_DIFFICULTY_BUTTONS);
				destroyButtons(settingsWindowUserColorButtons,SETTINGS_MENU_USER_COLOR_BUTTONS);
				destroyButtons(settingsWindowManageButtons,SETTINGS_MENU_MANAGE_BUTTONS);
				destroyGUIElements(settingsWindow, rend, tex, numTex);
				SDL_Quit();
				return QUIT_WINDOW;
			}
		}

		SDL_RenderPresent(rend);

		if(SDL_WaitEvent(&e) == 0){
			er = SDL_ERROR;
			destroyButtons(settingsWindowGameModeButtons,SETTINGS_MENU_GAME_MODE_BUTTONS);
			destroyButtons(settingsWindowDifficultyButtons,SETTINGS_MENU_DIFFICULTY_BUTTONS);
			destroyButtons(settingsWindowUserColorButtons,SETTINGS_MENU_USER_COLOR_BUTTONS);
			destroyButtons(settingsWindowManageButtons,SETTINGS_MENU_MANAGE_BUTTONS);
			destroyGUIElements(settingsWindow, rend, tex, numTex);
			SDL_Quit();
			return QUIT_WINDOW;
		}
		handleEventsSettingsMenuWindow(&e,&window, settings, settingsWindowGameModeButtons,SETTINGS_MENU_GAME_MODE_BUTTONS,
				settingsWindowDifficultyButtons, SETTINGS_MENU_DIFFICULTY_BUTTONS,
				settingsWindowUserColorButtons, SETTINGS_MENU_USER_COLOR_BUTTONS,
				settingsWindowManageButtons, SETTINGS_MENU_MANAGE_BUTTONS);
	}

	destroyButtons(settingsWindowGameModeButtons, SETTINGS_MENU_GAME_MODE_BUTTONS);
	destroyButtons(settingsWindowDifficultyButtons, SETTINGS_MENU_DIFFICULTY_BUTTONS);
	destroyButtons(settingsWindowUserColorButtons, SETTINGS_MENU_USER_COLOR_BUTTONS);
	destroyButtons(settingsWindowManageButtons, SETTINGS_MENU_MANAGE_BUTTONS);
	destroyGUIElements(settingsWindow, rend, tex, numTex);
	SDL_Quit();

	return window;
}


