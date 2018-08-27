
#ifndef GAMEWINDOW_H_
#define GAMEWINDOW_H_

#include <SDL.h>
#include <SDL_video.h>
#include "GUIAux.h"
#include "MainAux.h"


#define NUM_PLAYER_CHESS_TOOLS 16
#define GAME_MENU_BUTTONS 6
#define NUM_OF_SLOTS 5
#define CHESS_BOARD_CELL_SIZE 80
#define WHITE_PAWN_IMAGE "pictures/whitePawn.bmp"
#define WHITE_BISHOP_IMAGE "pictures/whiteBishop.bmp"
#define WHITE_KNIGHT_IMAGE "pictures/whiteKnight.bmp"
#define WHITE_ROOK_IMAGE "pictures/whiteRook.bmp"
#define WHITE_QUEEN_IMAGE "pictures/whiteQueen.bmp"
#define WHITE_KING_IMAGE "pictures/whiteKing.bmp"
#define BLACK_PAWN_IMAGE "pictures/blackPawn.bmp"
#define BLACK_BISHOP_IMAGE "pictures/blackBishop.bmp"
#define BLACK_KNIGHT_IMAGE "pictures/blackKnight.bmp"
#define BLACK_ROOK_IMAGE "pictures/blackRook.bmp"
#define BLACK_QUEEN_IMAGE "pictures/blackQueen.bmp"
#define BLACK_KING_IMAGE "pictures/blackKing.bmp"

/*
 * create chess board cell from
 * (x,y) coordinates on the window.
 */
SPCell createCellFromCoords(int x, int y);

/*
 * game window.
 */
SP_CHESS_GAME_GUI_WINDOW gameMenuWindow(SPChessGame* game, Settings* settings);


#endif /* GAMEWINDOW_H_ */
