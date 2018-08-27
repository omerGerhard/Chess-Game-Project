#include <stdio.h>
#include "SPChessParser.h"
#include <string.h>
#include <stdlib.h>

SPCommand createSPCommand(SP_COMMAND cmd, bool validArg, int arg, SPCell source, SPCell target, char* path){
	SPCommand spc;
	spc.cmd = cmd;
	spc.validArg = validArg;
	spc.arg = arg;
	spc.source = source;
	spc.target = target;
	strcpy(spc.path, path);
	return spc;
}

SP_COMMAND toSP_COMMAND(char *p){
	if(strcmp(p,"game_mode")==0)
		return SP_GAME_MODE;
	else if(strcmp(p,"difficulty")==0)
		return SP_DIFFICULTY;
	else if(strcmp(p,"user_color")==0)
		return SP_USER_COLOR;
	else if(strcmp(p,"load")==0)
		return SP_LOAD;
	else if(strcmp(p,"default")==0)
		return SP_DEFAULT;
	else if(strcmp(p,"print_settings")==0)
		return SP_PRINT_SETTINGS;
	else if(strcmp(p,"start")==0)
		return SP_START;
	else if(strcmp(p,"move")==0)
		return SP_MOVE;
	else if(strcmp(p,"get_moves")==0)
		return SP_GET_MOVES;
	else if(strcmp(p,"save")==0)
		return SP_SAVE;
	else if(strcmp(p,"undo")==0)
		return SP_UNDO;
	else if(strcmp(p,"quit")==0)
		return SP_QUIT;
	else if(strcmp(p,"reset")==0)
		return SP_RESET;
	else
		return SP_INVALID_LINE;
}

bool intToBool(int n){
	if(n)
		return true;
	return false;
}

//iterates over elements of 'arr' which contains 'nElements' of size 'sizeOfElement'
//and return true if func(arr[i]) == true for every 0<=i && i<= 'nElements' - 1.
int forall(void *arr, int nElements, int (*func)(void *)){
	char *p = arr;
	for(int i=0 ; i<nElements ; i++){
		p+=1;
		if(!func(p))
			return 0;
	}
	return 1;
}

//return true if 'd' is a digit and false else.
int isDigit(void *d){
	if('0'<= *(char *)d && *(char *)d <='9')
		return 1;
	return 0;
}

bool spParserIsInt(const char* str){
	bool b;
	int len = strlen(str);
	char *p, s[len+1];
	if(len == 0)
		return false;
	if(len == 1)
		return ((*str)>='0' && (*str)<='9');
	strcpy(s,str);
	if(strcmp(s, "-0")==0)
		return 0;
	p=s;
	if(!((*p)=='-' || ((*p)>='0' && (*p)<='9'))){
		return false;
	}
	b =(intToBool(forall(p,len-1,isDigit)));
	return b;
}


int checkStrIsIntInRange(const char* p, int lowBound, int highBound){
	int n;
	if(spParserIsInt(p)){
		n = atoi(p);
		if(lowBound<= n && n <= highBound)
			return n;
		return -1;
	}
	return -1;
}

int getValidArg(SP_COMMAND sp, char* p){
	if(sp == SP_GAME_MODE)
		return checkStrIsIntInRange(p, 1, 2);
	else if(sp == SP_DIFFICULTY)
		return checkStrIsIntInRange(p, 1, 5);
	else // sp == SP_USER_COLOR
		return checkStrIsIntInRange(p, 0, 1);
}

bool validCol(char c){
	return ('A' <= c && c <= 'H');
}

//fix problem here
SPCell getValidCell(const char* p){
	char oParen, cParen, first, comma, second, t[2];
	int row;
	int len = strlen(p);
	if(len != 5)
		return spCellCreate(0, 'A');
	oParen = *p;
	first = *(p+1);
	comma = *(p+2);
	second = *(p+3);
	cParen = *(p+4);
	t[0] = first;
	t[1] = 0;
	row = checkStrIsIntInRange(t, 1, 8);
	if(row != -1 && comma == ',' && validCol(second) && oParen == '<' && cParen == '>')
		return spCellCreate(row, second);
	return spCellCreate(0, 'A');
}

ARG_TYPE getArgType(SP_COMMAND sp){
	if(sp == SP_GAME_MODE || sp == SP_DIFFICULTY || sp == SP_USER_COLOR)
		return ARG;
	else if(sp == SP_MOVE)
		return TWO_CELL;
	else if (sp == SP_GET_MOVES)
		return ONE_CELL;
	else if (sp == SP_LOAD || sp == SP_SAVE)
		return PATH;
	else
		return NO_ARGS;
}

//for testing.
void printArgType(ARG_TYPE at){
	if(at == ARG)
		printf("ARG\n");
	else if(at == ONE_CELL)
		printf("ONE_CELL\n");
	else if(at == TWO_CELL)
		printf("TWO_CELL\n");
	else if(at == PATH)
		printf("PATH\n");
	else
		printf("NO_ARG\n");
}

SPCommand spParserPraseLine(const char* str){
	SPCommand spc;
	char *p,s[SP_MAX_LINE_LENGTH];
	ARG_TYPE argType;
	spc.validArg=false;
	int size = strlen(str);
	if(size>SP_MAX_LINE_LENGTH){
		spc.cmd = SP_INVALID_LINE;
		return spc;
	}
	strcpy(s,str);
	p = strtok(s," \t\n\r");
	if(p==NULL){
		spc.cmd = SP_INVALID_LINE;
		return spc;
	}
	spc.cmd = toSP_COMMAND(p);
	argType = getArgType(spc.cmd);
	if(argType != NO_ARGS){
		p=strtok(NULL," \t\n\r");
		if(p==NULL)
			return spc;
		if(argType == ARG){
			spc.arg = getValidArg(spc.cmd, p);
			spc.validArg = spc.arg == -1 ? false : true;
		}
		else if(argType == ONE_CELL){
			spc.source = getValidCell(p);
			spc.validArg = spc.source.row == 0 ? false : true;
		}
		else if(argType == TWO_CELL){
			spc.source = getValidCell(p);
			spc.validArg = spc.source.row == 0 ? false : true;
			p=strtok(NULL," \t\n\r");
			if(p != NULL){
				if (strcmp(p, "to") != 0)
					spc.validArg = false;
				else{
					p=strtok(NULL," \t\n\r");
					if(p != NULL){
						spc.target = getValidCell(p);
						spc.validArg = spc.target.row == 0 ? false : true;
					}
					else
						spc.validArg = false;
				}
			}
			else
				spc.validArg = false;

		}
		else if(argType == PATH){
			spc.validArg = true;
			strcpy(spc.path, p);
		}
	}
	return spc;
}

