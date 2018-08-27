
#include "settings.h"
#include "CHESSGame.h"
#include "MainAux.h"
#include <stdio.h>
#include <stdlib.h>


Settings* createSettings(){
	Settings* settings = (Settings *)malloc(sizeof(Settings));
	if(settings == NULL){
		er = MEM_ALLOC;
		return NULL;
	}
	settings->game_mode=1;
	settings->difficulty=2;
	settings->user_color=1;
	return settings;
}

void setDefault(Settings* settings){
	settings->game_mode=1;
	settings->difficulty=2;
	settings->user_color=1;
}

void destroySettings(Settings* s){
	free(s);
}

void setGameMode(Settings* settings , int game_mode){
	settings->game_mode=game_mode;
}

void setDifficulty(Settings* settings , int difficulty){
	settings->difficulty=difficulty;
}

void setUserColor(Settings* settings , int userColor){
	settings->user_color=userColor;
}

/*
 * prints the difficulty name according to paran 'diff'.
 */
void printDifficulty(int diff){
	switch(diff){
		case(1):
			printf("DIFFICULTY: amateur\n");
			break;
		case(2):
			printf("DIFFICULTY: easy\n");
			break;
		case(3):
			printf("DIFFICULTY: moderate\n");
			break;
		case(4):
			printf("DIFFICULTY: hard\n");
			break;
		default:
			printf("DIFFICULTY: expert\n");
			break;
	}
}

/*
 * prints the user_color name according to param 'color'.
 */
void printUserColor(int color){
	if(color==0)
		printf("USER_COLOR: black\n");
	else
		printf("USER_COLOR: white\n");
}


void printSettings(Settings* settings){
	printf("SETTINGS:\n");
	if(settings->game_mode==2)
		printf("GAME_MODE: 2-player\n");
	else{
		printf("GAME_MODE: 1-player\n");
		printDifficulty(settings->difficulty);
		printUserColor(settings->user_color);
	}
}


int getModeFromFile(char *line){
	if(line[11]=='1')
		return 1;
	return 2;
}


int getUserColorFromFile(char *line){
	if(line[12]=='b')
		return 0;
	return 1;
}


int getDifficultyFromFile(char *line){
	char c = line[14];
	if(c=='a')
		return 1;
	else if(c=='s')
		return 2;
	else if(c=='d')
		return 3;
	else if(c=='r')
		return 4;
	else
		return 5;
}



