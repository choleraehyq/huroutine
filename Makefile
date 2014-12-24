OBJ=huroutine.o
CC=clang
CFLAGS=-g -Wall

all : main

$(OBJ) : huroutine.h huroutine.c
	$(CC) huroutine.c -c $(CFLAGS) -o $(OBJ)
main : $(OBJ) main.c
	$(CC) main.c $(OBJ) $(CFLAGS) -o main

.PHONY : clean
clean:
	-rm main $(OBJ)
