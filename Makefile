OBJ= err.o huroutine.o linklist.o nbio.o channel.o uthread.o 
CC=clang
CFLAGS=-g -Wall -lpthread -lrt
LINKFLAGS=-c -g -Wall

.PHONY : all
all : main

err.o : err.c 
	$(CC) err.c $(LINKFLAGS) -o err.o
linklist.o : err.o linklist.c 
	$(CC) linklist.c $(LINKFLAGS) -o linklist.o 
huroutine.o : err.o linklist.o huroutine.c
	$(CC) huroutine.c $(LINKFLAGS) -o huroutine.o 
channel.o : err.o huroutine.o channel.c
	$(CC) channel.c $(LINKFLAGS) -o channel.o
nbio.o : err.o linklist.o huroutine.o nbio.c
	$(CC) nbio.c $(LINKFLAGS) -o nbio.o
uthread.o : err.o huroutine.o linklist.o  uthread.c
	$(CC) uthread.c $(LINKFLAGS) -o uthread.o 

main : $(OBJ) main.c
	$(CC) main.c $(OBJ) $(CFLAGS) -o main

.PHONY : clean
clean:
	-rm main $(OBJ)
	-rm *~
