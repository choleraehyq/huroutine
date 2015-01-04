OBJ=huroutine.o linklist.o uthread.o
SRC=huroutine.c linklist.c uthread.c
CC=clang
CFLAGS=-g -Wall -lpthread -lrt
LINKFLAGS=-g -Wall

.PHONY : all
all : main

huroutine.o : huroutine.c
	$(CC) huroutine.c -c $(LINKFLAGS) -o huroutine.o
linklist.o : linklist.c
	$(CC) linklist.c -c $(LINKFlAGS) -o linklist.o 
uthread.o : huroutine.o linklist.o  uthread.c
	$(CC) uthread.c -c $(LINKFLAGS) -o uthread.o
main : $(OBJ) main.c
	$(CC) main.c $(OBJ) $(CFLAGS) -o main

.PHONY : clean
clean:
	-rm main $(OBJ)
	-rm *~
