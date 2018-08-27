

#include "MainAux.h"
#include "GUIAux.h"
#include "CHESSGame.h"
#include "Minimax.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


GAME_ERROR er = NONE;
bool console;


int main(int argc, char* argv[]){
	if(argc > 1){
		if(strcmp(argv[1],"-g") == 0){
			console = false;
			mainGUI();
		}
		else if(strcmp(argv[1],"-c") == 0){
			console = true;
			mainConsole();
		}
	}
	else{
		console = true;
		mainConsole();
	}
	return 0;
}

