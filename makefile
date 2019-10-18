all: run
main: main.c
	gcc -std=c89 -g main.c -o main -lm

merge: merge.c
	gcc -std=c89 -g merge.c -o merge -lm
run: main merge input-file
	clear ; ./main < input-file
