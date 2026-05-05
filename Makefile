CC = gcc
CFLAGS = -g -Wall -Wextra -Wpedantic
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

prog: main.o source.o joueur.o highscore.o minimap.o save_system.o puzzle_game/puzzle
	$(CC) main.o source.o joueur.o highscore.o minimap.o save_system.o -o prog $(LIBS)

main.o: main.c header.h highscore.h minimap/minimap.h save_system.h
	$(CC) -c main.c $(CFLAGS)

source.o: source.c header.h
	$(CC) -c source.c $(CFLAGS)

joueur.o: joueur.c header.h
	$(CC) -c joueur.c $(CFLAGS)

highscore.o: highscore.c highscore.h
	$(CC) -c highscore.c $(CFLAGS)

minimap.o: minimap/minimap.c minimap/minimap.h
	$(CC) -c minimap/minimap.c $(CFLAGS)

save_system.o: save_system.c save_system.h header.h
	$(CC) -c save_system.c $(CFLAGS)

puzzle_game/puzzle: puzzle_game/main.c puzzle_game/game.c puzzle_game/game.h puzzle_game/assets.h
	$(MAKE) -C puzzle_game

clean:
	rm -f prog main.o source.o joueur.o highscore.o minimap.o save_system.o
	$(MAKE) -C puzzle_game clean
