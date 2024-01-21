all: myshell

myshell: myshell.o
	gcc -m32 -g -Wall -o myshell myshell.o

myshell.o: myshell.c
	gcc -m32 -g -Wall -c -o myshell.o myshell.c

.PHONY: clean

clean:
	rm -f *.o myshell
