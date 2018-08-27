
#ifndef CELL_H_
#define CELL_H_

#include <stdbool.h>

typedef struct cell_t{
	int row;
	char col;
}SPCell;

SPCell spCellCreate(int row, char col);

#endif /* CELL_H_ */
