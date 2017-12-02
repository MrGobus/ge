CC = gcc
CFLAGS = -Wall `pkg-config --cflags freetype2`
LFLAGS = -lglfw3 -lglew32 -lopengl32 -lIL -lfreetype -liconv
TARGET = main.exe

default: clear
	$(CC) $(CFLAGS) -c src/ge.c
	$(CC) $(CFLAGS) -c main.c
	$(CC) ge.o main.o $(LFLAGS) -o $(TARGET)
	
clear:
	rm -f *.o
	rm -f $(TARGET)