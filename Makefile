CC=g++
CFLAGS=-Wall -std=c++11 -g

main: main.c model.o tgaimage.o
	$(CC) $(CFLAGS) main.c model.o tgaimage.o -o main 

model.o: model.c model.h
	$(CC) $(CFLAGS) -c model.c

tgaimage.o: tgaimage.cpp tgaimage.h
	$(CC) $(CFLAGS) -c tgaimage.cpp

clean:
	rm main *.o

run: main
	./main && eog output.tga