all: sixth

sixth: sixth.c
	gcc -Wall -Werror -fsanitize=address sixth.c -o sixth -lm

clean:
	rm -f sixth
