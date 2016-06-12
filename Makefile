CC=g++
CFLAGS=-Wall -std=c++11 -g -Wno-write-strings -Wno-narrowing -O1

main: main.c model.o tgaimage.o geometry.o matrix.o our_gl.o
	$(CC) $(CFLAGS) main.c model.o tgaimage.o geometry.o matrix.o  our_gl.o -o main 

model.o: model.c model.h
	$(CC) $(CFLAGS) -c model.c

tgaimage.o: tgaimage.cpp tgaimage.h
	$(CC) $(CFLAGS) -c tgaimage.cpp

geometry.o: geometry.c geometry.h
	$(CC) $(CFLAGS) -c geometry.c

matrix.o: matrix.c matrix.h
	$(CC) $(CFLAGS) -c matrix.c

our_gl.o: our_gl.c our_gl.h
	$(CC) $(CFLAGS) -c our_gl.c

clean:
	rm main *.o

rebuild: clean main

run: main
	./main && eog output.tga