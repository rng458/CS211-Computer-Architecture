all: fifth

fifth: fifth.c
	gcc -Wall -Werror -fsanitize=address fifth.c -o fifth -lm

clean:
	rm -f fifth
