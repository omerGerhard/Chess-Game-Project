
#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <stdio.h>
#include "CHESSGame.h"

typedef struct settings_t{
	int game_mode;
	int difficulty;
	int user_color;
}Settings;


/*
 * creates a pointer to an occurence of a new Settings object.
 */
Settings* createSettings();

/*
 * sets all fields of 'settings' to their defaults -
 * sets game_mode to 1 , difficulty to 2 and user_color to 1.
 */
void setDefault(Settings* settings);

/*
 * free the object settings.
 */
void destroySettings(Settings* s);

/*
 * changes the field game_mode of 'settings' to 'gameMode'.
 */
void setGameMode(Settings* settings , int gameMode);

/*
 * changes the field difficulty of 'settings' to 'difficulty'.
 */
void setDifficulty(Settings* settings , int difficulty);

/*
 * changes the field user_color of 'settings' to 'userColor'.
 */
void setUserColor(Settings* settings , int userColor);


/*
 * prints current settings according to 'settings' fields.
 */
void printSettings(Settings* settings);


/*
 * gets the line that includes the game_mode parameter which read from a file and
 * parses game_mode from it.
 */
int getModeFromFile(char *line);


/*
 * gets the line that includes the user_color parameter which read from a file and
 * parses user_color from it.
 */
int getUserColorFromFile(char *line);


/*
 * gets the line that includes the difficulty parameter which read from a file and
 * parses difficulty from it.
 */
int getDifficultyFromFile(char *line);



#endif /* SETTINGS_H_ */
