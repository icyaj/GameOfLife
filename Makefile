all: gol.o libgol.so gameoflife
gol.o: gol.c
	gcc -c gol.c -fPIC -Wall -Wextra -pedantic -std=c11
libgol.so:  gol.o
	gcc -shared gol.o -o libgol.so -Wall -Wextra -pedantic -std=c11
gameoflife: gameoflife.c
	gcc -o gameoflife gameoflife.c -lgol -L. -Wall -Wextra -pedantic -std=c11 -lm
clean:
	rm -rf gol.o libgol.so gameoflife