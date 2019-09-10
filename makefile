DEPENDS = lib/linenoise.h lib/linenoise.c src/bf.c src/bf.h src/bf_vars.h src/ui.c src/error_handler.c src/error_handler.h src/intern_errors.h

bfi: $(DEPENDS)
	$(CC) -Wall -Ofast -o bfi lib/linenoise.c src/bf.c src/error_handler.c src/ui.c

clean:
	rm bf*
	rm a.c
	rm a.out

debug: $(DEPENDS)	
	# Used if you want debugging features on
	$(CC) -Wall -Wextra -O0 -g -DDEBUGGING -o bfi.debugging lib/linenoise.c src/bf.c src/error_handler.c src/ui.c

