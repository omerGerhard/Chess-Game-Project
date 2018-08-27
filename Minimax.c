#include <stdio.h>
#include <stdlib.h>
#include "CHESSGame.h"
#include "Minimax.h"
#include "MainAux.h"
#include "SPArrayList.h"
#include <limits.h>


/*
 * create MinimaxMove with value = 'value', source = 'src' and target = 'target
 * and return this MinimaxMove.
 */
MinimaxMove createMinimaxMove(int value, SPCell src, SPCell target){
	MinimaxMove move;
	move.value = value;
	move.source = src;
	move.target = target;
	return move;
}


/*
 * creates new Node pointer which represents node in the minmax tree
 * with the game board 'game'. the function return this pointer.
 */
Node* createNode(SPChessGame* game){
	Node* node = (Node *)malloc(sizeof(Node));
	if(node == NULL){
		er = MEM_ALLOC;
		return NULL;
	}
	node->data = game;
	node->next = NULL;
	return node;
}


/*
 * create pointer to empty list of Nodes with a dummy head
 * and return this pointer.
 */
List* createList(){
	List* lst = (List *)malloc(sizeof(List));
	if(lst == NULL){
		er = MEM_ALLOC;
		return NULL;
	}
	Node* head = createNode(NULL);  //dummy head
	if(er != NONE){
		free(lst);
		return NULL;
	}
	lst->head = head;
	return lst;
}


/*
 * insert 'node' to the beginning of the list 'lst'
 * (next after the dummy head)
 */
void insertFirst(List* lst , Node* node){
	node->next = lst->head->next;
	lst->head->next = node;
}


/*
 * frees all node's and its next nodes allocated memory.
 */
void destroyNode(Node* node){
	if(node->next == NULL){
		spChessGameDestroy(node->data);
		free(node);
		return;
	}
	destroyNode(node->next);
	spChessGameDestroy(node->data);
	free(node);
}


/*
 * frees all list allocated memory.
 */
void destroyList(List* lst){
	if(lst!=NULL){
		destroyNode(lst->head);
		free(lst);
	}
}


/*
 * return pointer to the list of all possible children
 * of the Node 'node' in the minmax tree.
 */
List* createChildrenList(Node* node){
	List* list = createList();
	if(er != NONE)
		return NULL;
	Node* child;
	int** matrix = allocateNewMatrix(8,8);
	if(er != NONE){
		destroyList(list);
		return NULL;
	}
	SPChessGame *game = node->data , *copy;
	char tool , player=game->currentPlayer;
	SPCell source , target;
	for(int j=7; j>=0; j--){
		for(int i=7; i>=0; i--){
			tool = game->gameBoard[i][j];
			if(player==toolToPlayer(tool)){
				source = spCellCreate(i+1,j+'A');
				validMoves(game,source,tool,matrix);
				if(er != NONE){
					destroyMatrix(matrix,8);
					destroyList(list);
					return NULL;
				}
				for(int n=7; n>=0; n--){
					for(int m=7; m>=0; m--){
						if(matrix[m][n]!=0){
							target = spCellCreate(m+1,n+'A');
							copy = spChessGameCopy(game);
							if(er == NONE){
								child = createNode(copy);
								if(child != NULL){
									spChessGameSetMove(child->data, source, target);
									if(er == NONE)
										insertFirst(list,child);
									else
										destroyNode(child);
								}
							}
						}
					}
				}
				setToZero(matrix);
			}
		}
	}
	destroyMatrix(matrix,8);
	return list;
}



/*
 * return the score sign e.g negative or positive
 * for the score of the leafs in the minmax tree.
 */
int scoreSign(bool isMax){
	if(isMax)
		return -1;
	return 1;
}


/*
 * return the score of each tool for the scoring function.
 */
int toolScore(char tool){
	if(tool=='m' || tool=='M')
		return 1;
	else if(tool=='n' || tool=='N' || tool=='b' || tool=='B')
		return 3;
	else if(tool=='r' || tool=='R')
		return 5;
	else if(tool=='q' || tool=='Q')
		return 9;
	else if(tool=='k' || tool=='K')
		return 100;
	else
		return 0;
}


/*
 * return the score of the tool 'tool' based on the 'player'
 * who played the last turn for the scoring function.
 */
int getScore(char tool, char player){
	char toolPlayer = toolToPlayer(tool);
	int result, sign = player == toolPlayer ? 1 : -1;
	result=toolScore(tool);
	return result * sign;
}


/*
 * calculates the score of the game board 'game' for the leaves of the minmax tree.
 */
int scoringFunction(SPChessGame* game , bool isMax){
	char player = changeTurn(game->currentPlayer);
	int sign = scoreSign(isMax);
	SP_CHESS_CHECK_WINNER_MESSAGE gameOver = spChessCheckWinner(game);
	if(er != NONE)
		return 0;
	if(gameOver==DRAW)
		return 0;
	else if(gameOver==CHECKMATE_WHITE_PLAYER && player==SP_CHESS_GAME_WHITE_PLAYER)
		return 1000*sign;
	else if(gameOver==CHECKMATE_BLACK_PLAYER && player==SP_CHESS_GAME_BLACK_PLAYER)
		return 1000*sign;
	int score = 0;
	char tool;
	for(int i=0; i<SP_CHESS_GAME_N_ROWS; i++){
		for(int j=0; j<SP_CHESS_GAME_N_COLUMNS; j++){
			tool = game->gameBoard[i][j];
			score += getScore(tool , player);
		}
	}
	return sign*score;
}


/*
 * recieve game board 'game' and create MinimaxMove
 * based on the last move form the game history.
 */
MinimaxMove createMoveFromHistory(SPChessGame* src, int score){
	int actualSize = src->history->actualSize;
	if(actualSize == 0){
		SPCell source = spCellCreate(0,'A'); // default cell
		SPCell target = spCellCreate(0,'A'); // default cell
		MinimaxMove move = createMinimaxMove(score,source,target);
		return move;
	}
	SPArrayCell ac = src->history->elements[actualSize-1];
	SPCell source = ac.source;
	SPCell target = ac.target;
	MinimaxMove move = createMinimaxMove(score,source,target);
	return move;
}


/*
 * return the bigger value move between 'move' and 'childMove'.
 */
MinimaxMove maxValueMove(SPChessGame* src,MinimaxMove move, MinimaxMove childMove){
	if(childMove.value > move.value){
		return createMoveFromHistory(src,childMove.value);
	}
	return move;
}


/*
 * return the bigger value between n1 and n2.
 */
int maxValue(int n1 , int n2){
	if(n2>n1)
		return n2;
	return n1;
}


/*
 * return the smaller value move between 'move' and 'childMove'.
 */
MinimaxMove minValueMove(SPChessGame* src,MinimaxMove move, MinimaxMove childMove){
	if(childMove.value < move.value){
		return createMoveFromHistory(src,childMove.value);
	}
	return move;
}


/*
 * return the smaller value between n1 and n2.
 */
int minValue(int n1 , int n2){
	if(n1>n2)
		return n2;
	return n1;
}


/*
 * the recursive function for creating the minmax tree.
 */
MinimaxMove minimaxStep(Node* node,int depth,int a,int b,bool isMax){
	MinimaxMove move , childMove;
	Node* child;
	int score;
	SP_CHESS_CHECK_WINNER_MESSAGE mes = spChessCheckWinner(node->data);
	if(er != NONE)
		return createMinimaxMove(0,spCellCreate(0,'0'),spCellCreate(0,'0'));
	if(depth == 0 || isGameOver(mes)){
		score = scoringFunction(node->data,isMax);
		if(er != NONE)
			return createMinimaxMove(0,spCellCreate(0,'0'),spCellCreate(0,'0'));
		move = createMoveFromHistory(node->data , score);
		return move;
	}
	List* list = createChildrenList(node);
	if(er != NONE){
		destroyList(list);
		return createMinimaxMove(0,spCellCreate(0,'0'),spCellCreate(0,'0'));
	}
	if(isMax){
		move = createMinimaxMove(INT_MIN,spCellCreate(0,'0'),spCellCreate(0,'0'));
		child = list->head->next;
		while(child != NULL){
			childMove = minimaxStep(child,depth-1,a,b,false);
			if(childMove.source.row != 0){
				move = maxValueMove(child->data,move,childMove);
				a = maxValue(a,move.value);
				if(a>=b)
					break;
			}
			child=child->next;
		}
		destroyList(list);
		return move;
	}
	else{
		move = createMinimaxMove(INT_MAX,spCellCreate(0,'0'),spCellCreate(0,'0'));
		child = list->head->next;
		while(child != NULL){
			childMove = minimaxStep(child,depth-1,a,b,true);
			if(childMove.source.row != 0){
				move = minValueMove(child->data,move,childMove);
				b = minValue(b,move.value);
				if(a>=b)
					break;
			}
			child=child->next;
		}
		destroyList(list);
		return move;
	}
}



MinimaxMove suggestMove(SPChessGame* src , int depth){
	Node* node = createNode(spChessGameCopy(src));
	if(node == NULL){
		er = MEM_ALLOC;
		return createMinimaxMove(0,spCellCreate(0,'0'),spCellCreate(0,'0'));
	}
	MinimaxMove move = minimaxStep(node,depth,INT_MIN,INT_MAX,true);
	if(er != NONE){
		destroyNode(node);
		return createMinimaxMove(0,spCellCreate(0,'0'),spCellCreate(0,'0'));
	}
	destroyNode(node);
	return move;
}

