
#ifndef MINIMAX_H_
#define MINIMAX_H_

typedef struct sp_chess_game_list_node_t{
	SPChessGame* data;
	struct sp_chess_game_list_node_t* next;

}Node;

typedef struct sp_chess_game_linked_list_t{
	Node* head;
}List;

typedef struct sp_chess_game_minimax_move_t{
	int value;
	SPCell source;
	SPCell target;
}MinimaxMove;


/*
 * return the suggested move for the current player based on the minmax
 * with level 'depth' (the maximal depth of the minmax tree).
 */
MinimaxMove suggestMove(SPChessGame* src , int depth);


#endif /* MINIMAX_H_ */
