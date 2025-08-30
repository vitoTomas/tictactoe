include config.ini

main:
	gcc -DDIM=3 -DMAX_DEPTH=2 -march=native main.c -o tictac

debug:
	gcc -DDEBUG -DDIM=5 -DUSEC=50 -DMAX_DEPTH=($MAX_DEPTH_VAR) main.c -o tictac

clean:
	rm -f tictac
