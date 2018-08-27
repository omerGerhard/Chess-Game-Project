
#include "cell.h"


SPCell spCellCreate(int row, char col){
	SPCell c;
	c.row = row;
	c.col = col;
	return c;
}

//could be redundant
bool isValidCell(SPCell cell){
	if(!(1<=cell.row && cell.row<=8))
		return false;
	if(!('A'<=cell.col && cell.col<='H'))
		return false;
	return true;
}

