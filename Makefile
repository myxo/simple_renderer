CC=g++
CFLAGS=-Wall -std=c++11 -g -Wno-write-strings

main: main.c model.o tgaimage.o geometry.o
	$(CC) $(CFLAGS) main.c model.o tgaimage.o geometry.o -o main 

model.o: model.c model.h
	$(CC) $(CFLAGS) -c model.c

tgaimage.o: tgaimage.cpp tgaimage.h
	$(CC) $(CFLAGS) -c tgaimage.cpp

geometry.o: geometry.c geometry.h
	$(CC) $(CFLAGS) -c geometry.c

clean:
	rm main *.o

rebuild: clean main

run: main
	./main && eog output.tga