#include <stdbool.h>
#include "cell.h"

#ifndef SPCHESSPRASER_H_
#define SPCHESSPRASER_H_

//specify the maximum line length
#define SP_MAX_LINE_LENGTH 2048

//a type used to represent a command
typedef enum {
	SP_GAME_MODE,
	SP_DIFFICULTY,
	SP_USER_COLOR,
	SP_LOAD,
	SP_DEFAULT,
	SP_PRINT_SETTINGS,
	SP_START,
	SP_MOVE,
	SP_GET_MOVES,
	SP_SAVE,
	SP_UNDO,
	SP_QUIT,
	SP_RESET,
	SP_INVALID_LINE,
	SP_GAME_OVER
} SP_COMMAND;

//a new type that is used to encapsulate a parsed line
typedef struct command_t {
	SP_COMMAND cmd;
	bool validArg; //is set to true if the line contains a valid argument
	int arg;
	SPCell source;
	SPCell target;
	char path[SP_MAX_LINE_LENGTH];
} SPCommand;

//a type used to represent the argument type of a command
typedef enum {
	ARG,
	ONE_CELL,
	TWO_CELL,
	PATH,
	NO_ARGS
} ARG_TYPE;

SPCommand createSPCommand(SP_COMMAND cmd, bool validArg, int arg, SPCell source, SPCell target, char* path);

/**
 * return the coresponding command for the string in argument p.
 * return SP_INVALID_LINE if the string does not represent a valid command.
 */
SP_COMMAND toSP_COMMAND(char *p);

/**
 * return false if n == 0 and true otherwise.
 */
bool intToBool(int n);

//iterates over elements of 'arr' which contains 'nElements' of size 'sizeOfElement'
//and return true if func(arr[i]) == true for every 0<=i && i<= 'nElements' - 1.
int forall(void *arr, int nElements, int (*func)(void *));

//return true if 'd' is a digit and false else.
int isDigit(void *d);

/**
 * Checks if a specified string represents a valid integer. It is recommended
 * to use this function prior to calling the standard library function atoi.
 *
 * @return
 * true if the string represents a valid integer, and false otherwise.
 */
bool spParserIsInt(const char* str);

/*
 * Checks if a specified string 'p' represents a valid integer (different than -1) which
 * apply that is value is bigger or equals to 'lowBound' and is smaller or equals to 'highBound'.
 * returns the integer value if it is a valid one, -1 otherwise.
 */
int checkStrIsIntInRange(const char* p, int lowBound, int highBound);

/*
 * for each of the commands that apply ARG_TYPE = ARG
 * check if the parameter arg of the command is a valid one
 * if it is return it's value, otherwise return -1.
 */
int getValidArg(SP_COMMAND sp, char* p);

/*
 * returns true if the char c represents a valid column of the game board
 * otherwise returns false.
 */
bool validCol(char c);

/*
 * Checks if the specified string 'p' is a valid input of a cell
 * e.g "<row,column>", if it does it returns a cell with the corresponding value
 * otherwise return a cell with wrong values which is row value of 0.
 */
SPCell getValidCell(const char* p);

/**
 * return the corresponding ARG_TYPE for the command in argument 'sp'.
 * return NO_ARGS if the command does not contain any arguments.
 */
ARG_TYPE getArgType(SP_COMMAND sp);

/**
 * Parses a specified line. If the line is a command which has an integer or source/target
 * cells argument or path to file then the argument is parsed and is saved in the field 'arg' or 'source'/'target or 'path' and the
 * field validArg is set to true. In any other case then 'validArg' is set to
 * false and the value 'arg' or 'source'/'target or 'path' is undefined
 *
 * @return
 * A parsed line such that:
 *   cmd - contains the command type, if the line is invalid then this field is
 *         set to INVALID_LINE
 *   validArg - is set to true if the command is add_disc and the integer argument
 *              is valid
 *   arg - the integer argument in case validArg is set to true and the ARG_TYPE of the command is ARG.
 *   source - the cell argument in case validArg is set to true and the ARG_TYPE of the command is ONE_CELL or TWO_CELL.
 *   target - the cell argument in case validArg is set to true and the ARG_TYPE of the command is TWO_CELL.
 *   path - the path argument in case validArg is set to true and the ARG_TYPE of the command is PATH.
 */
SPCommand spParserPraseLine(const char* str);


#endif /* SPCHESSPRASER_H_ */
