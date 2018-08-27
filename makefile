CC = gcc
OBJS = main.o MainAux.o CHESSGame.o Minimax.o settings.o SPArrayList.o SPChessParser.o cell.o GameWindow.o GUIAux.o MainMenu.o SaveWindow.o SettingsMenuWindow.o ToolButton.o Button.o
EXEC = chessprog
COMP_FLAG = -std=c99 -Wall -Wextra -Werror -pedantic-errors
SDL_COMP_FLAG = -I/usr/local/lib/sdl_2.0.5/include/SDL2 -D_REENTRANT

SDL_LIB = -L/usr/local/lib/sdl_2.0.5/lib -Wl,-rpath,/usr/local/lib/sdl_2.0.5/lib -Wl,--enable-new-dtags -lSDL2 -lSDL2main -lm


all: $(EXEC)
$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(SDL_LIB) -o $@
main.o: main.c MainAux.h CHESSGame.h Minimax.h GUIAux.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c main.c
MainAux.o: MainAux.c MainAux.h CHESSGame.h SPChessParser.h settings.h Minimax.h
	$(CC) $(COMP_FLAG) -c MainAux.c
CHESSGame.o: CHESSGame.c CHESSGame.h cell.h MainAux.h settings.h
	$(CC) $(COMP_FLAG) -c CHESSGame.c
GameWindow.o: GameWindow.c GameWindow.h ToolButton.h GUIAux.h MainAux.h MainMenu.h cell.h SaveWindow.h Button.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c GameWindow.c
GUIAux.o: GUIAux.c GUIAux.h Button.h MainAux.h MainMenu.h SettingsMenuWindow.h GameWindow.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c GUIAux.c
MainMenu.o: MainMenu.c MainMenu.h  MainAux.h SaveWindow.h Button.h GUIAux.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c MainMenu.c
Minimax.o: Minimax.c Minimax.h CHESSGame.h MainAux.h SPChessParser.h settings.h
	$(CC) $(COMP_FLAG) -c Minimax.c
SaveWindow.o: SaveWindow.c SaveWindow.h GUIAux.h CHESSGame.h settings.h Button.h MainAux.h MainMenu.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c SaveWindow.c
settings.o: settings.c settings.h CHESSGame.h MainAux.h
	$(CC) $(COMP_FLAG) -c settings.c
SettingsMenuWindow.o: SettingsMenuWindow.c SettingsMenuWindow.h GUIAux.h MainMenu.h Button.h MainAux.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c SettingsMenuWindow.c
SPArrayList.o: SPArrayList.c SPArrayList.h cell.h MainAux.h
	$(CC) $(COMP_FLAG) -c SPArrayList.c
SPChessParser.o: SPChessParser.c SPChessParser.h SPArrayList.h
	$(CC) $(COMP_FLAG) -c SPChessParser.c
ToolButton.o: ToolButton.c ToolButton.h GUIAux.h GameWindow.h MainAux.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c ToolButton.c
cell.o: cell.c cell.h
	$(CC) $(COMP_FLAG) -c cell.c
Button.o: Button.c Button.h GUIAux.h MainAux.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c Button.c
clean:
	rm -f $(OBJS) $(EXEC)