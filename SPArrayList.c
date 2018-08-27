

#include "SPArrayList.h"
#include "cell.h"
#include "MainAux.h"
#include <stdio.h>
#include <stdlib.h>



SPArrayCell spArrayCellCreate(int srcRow , char srcCol, int dstRow, char dstCol, char tool, char eatenTool){
	SPArrayCell cell;
	cell.source = spCellCreate(srcRow,srcCol);
	cell.target = spCellCreate(dstRow,dstCol);
	cell.tool = tool;
	cell.eatenTool = eatenTool;
	return cell;
}

SPArrayList* spArrayListCreate(int maxSize){
	SPArrayList *al = (SPArrayList *)malloc(sizeof(SPArrayList));
	if(al==NULL){
		er=MEM_ALLOC;
		return NULL;
	}
	al->elements=(SPArrayCell *)calloc(maxSize,sizeof(SPArrayCell));
	if(al->elements==NULL){
		free(al);
		er=MEM_ALLOC;
		return NULL;
	}
	al->actualSize = 0;
	al->maxSize = maxSize;
	return al;
}

SPArrayList* spArrayListCopy(SPArrayList* src){
	if(src==NULL)
		return NULL;
	SPArrayList *alCopy = spArrayListCreate(src->maxSize);
	if(alCopy==NULL)
		return NULL;
	alCopy->actualSize = src->actualSize;
	for(int i=0; i<alCopy->actualSize; i++)
		alCopy->elements[i] = src->elements[i];
	return alCopy;
}

void spArrayListDestroy(SPArrayList* src){
	if(src!=NULL){
		free(src->elements);
		free(src);
	}
}


SP_ARRAY_LIST_MESSAGE spArrayListAddLast(SPArrayList* src, SPArrayCell elem){
	if(src==NULL)
		return SP_ARRAY_LIST_INVALID_ARGUMENT;
	if(src->actualSize==src->maxSize){
		for(int i=0; i<src->actualSize-1; i++)
			src->elements[i]=src->elements[i+1];
		src->elements[src->actualSize-1]=elem;
		return SP_ARRAY_LIST_SUCCESS;
	}
	else{
		src->elements[src->actualSize]=elem;
		src->actualSize++;
		return SP_ARRAY_LIST_SUCCESS;
	}
}


SP_ARRAY_LIST_MESSAGE spArrayListRemoveLast(SPArrayList* src){
	if(src==NULL)
		return SP_ARRAY_LIST_INVALID_ARGUMENT;
	else if(src->actualSize==0)
		return SP_ARRAY_LIST_EMPTY;
	else{
		src->actualSize--;
		return SP_ARRAY_LIST_SUCCESS;
	}
}


SPArrayCell spArrayListGetLast(SPArrayList* src){
	if(src==NULL || src->actualSize==0){
		SPArrayCell t = spArrayCellCreate(0 , 'A', 0, 'A', '_', '_');
		return t;
	}
	return src->elements[src->actualSize-1];
}





