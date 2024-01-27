all: myshell

myshell: myshell.o LineParser.o
	gcc -m32 -g -Wall -o myshell myshell.o LineParser.o

Looper: looper.o
	gcc -m32 -g -Wall -o looper looper.o

myshell.o: myshell.c LineParser.h
	gcc -m32 -g -Wall -c -o myshell.o myshell.c

LineParser.o: LineParser.c LineParser.h
	gcc -m32 -g -Wall -c -o LineParser.o LineParser.c

looper.o: looper.c
	gcc -m32 -g -Wall -c -o looper.o looper.c


.PHONY: clean

clean:
	rm -f *.o *.txt myshell