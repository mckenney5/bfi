bfi: lib/linenoise.h lib/linenoise.c src/bfi.c
	$(CC) -Wall -Ofast -o bfi lib/linenoise.c src/bfi.c

bf2c: src/bf2c.c
	$(CC) -Wall -Ofast -o bf2c src/bf2c.c

clean:
	rm bf*
	rm a.c
	rm a.out

debug:	src/bfi.c lib/linenoise.c lib/linenoise.h
	# Used if you want debugging features on
	$(CC) -Wall -Wextra -O0 -g -DDEBUGGING -o bfi.debugging lib/linenoise.c src/bfi.c

