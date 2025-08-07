main:
	gcc main.c -o tictac

debug:
	gcc -DDEBUG -DUSEC=100e3 -DMAX_DEPTH=2 main.c -o tictac

clean:
	rm -f tictac
