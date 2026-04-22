prog: main.o game.o
	gcc main.o game.o -o prog -lSDL2 -g

main.o: main.c
	gcc -c main.c -g

game.o: game.c game.h entities.h
	gcc -c game.c -g

clean:
	rm -f *.o prog
