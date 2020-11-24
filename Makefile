all: build run


build: functii.c interpret.c lib.h main.c
	gcc -m32 functii.c main.c interpret.c coada.c lista_generica.c -o tema3 -Wall -Wextra -lm -g

run:
	./tema3 input.html input output


af:
	cat output

clean:
	rm -rf tema3

valgrind:
	valgrind --leak-check=full --error-exitcode=1 ./tema3 input.html input output
