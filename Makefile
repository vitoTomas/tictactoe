main:
	gcc -DDIM=3 -DMAX_DEPTH=2 -march=native main.c -o tictac

debug:
	gcc -DDEBUG -DDIM=5 -DUSEC=50 -DMAX_DEPTH=5 main.c -o tictac

clean:
	rm -f tictac
