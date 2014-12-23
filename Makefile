OBJ = huroutine.o
OBJ : huroutine.h huroutine.c
    cc huroutine.c -c -g
main : $(OBJ)
    cc main.c $(OBJ) -o main
clean:
    -rm main $(OBJ)
