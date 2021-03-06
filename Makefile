CC=g++
CFLAGS=-Wall -g -std=c++11 -Wno-write-strings -Wno-narrowing -O0

main: main.c model.o tgaimage.o geometry.o matrix.o our_gl.o shader.o
	$(CC) $(CFLAGS) main.c model.o tgaimage.o geometry.o matrix.o  our_gl.o shader.o -o main 

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

shader.o: shader.c shader.h
	$(CC) $(CFLAGS) -c shader.c

clean:
	rm main *.o

rebuild: clean main

run: main
	./main && eog output.tga