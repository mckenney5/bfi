bfi: lib/linenoise.h lib/linenoise.c src/bf.c src/bf.h src/bf_vars.h src/ui.c
	$(CC) -Wall -Ofast -o bfi lib/linenoise.c src/bf.c src/ui.c

bf2c: src/bf2c.c
	$(CC) -Wall -Ofast -o bf2c src/bf2c.c

clean:
	rm bf*
	rm a.c
	rm a.out

debug:	lib/linenoise.h lib/linenoise.c src/bf.c src/bf.h src/bf_vars.h src/ui.c
	# Used if you want debugging features on
	$(CC) -Wall -Wextra -O0 -g -DDEBUGGING -o bfi.debugging lib/linenoise.c src/bf.c src/ui.c

