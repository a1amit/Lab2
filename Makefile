all: myshell mypipe

myshell: myshell.o LineParser.o
	gcc -m32 -g -Wall -o myshell myshell.o LineParser.o

Looper: looper.o
	gcc -m32 -g -Wall -o looper looper.o

mypipe: mypipe.o
	gcc -m32 -g -Wall -o mypipe mypipe.o

myshell.o: myshell.c LineParser.h
	gcc -m32 -g -Wall -c -o myshell.o myshell.c

LineParser.o: LineParser.c LineParser.h
	gcc -m32 -g -Wall -c -o LineParser.o LineParser.c

looper.o: looper.c
	gcc -m32 -g -Wall -c -o looper.o looper.c

mypipe.o: mypipe.c
	gcc -m32 -g -Wall -c -o mypipe.o mypipe.c


.PHONY: clean

clean:
	rm -f *.o *.txt myshell