OBJ=huroutine.o linklist.o uthread.o
SRC=huroutine.c linklist.c uthread.c
CC=clang
CFLAGS=-g -Wall -lpthread -lrt
LINKFLAGS=-c -g -Wall

.PHONY : all
all : main

linklist.o : linklist.c
	$(CC) linklist.c $(LINKFLAGS) -o linklist.o 
huroutine.o : linklist.o huroutine.c
	$(CC) huroutine.c $(LINKFLAGS) -o huroutine.o 
uthread.o : huroutine.o linklist.o  uthread.c
	$(CC) uthread.c $(LINKFLAGS) -o uthread.o 

main : $(OBJ) main.c
	$(CC) main.c $(OBJ) $(CFLAGS) -o main

.PHONY : clean
clean:
	-rm main $(OBJ)
	-rm *~
