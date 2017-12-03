CC = gcc
LIB_DIR = lib
INCLUDE_DIR = include
CFLAGS = -I$(INCLUDE_DIR) -Wall `pkg-config --cflags freetype2` -O3
LFLAGS = -L$(LIB_DIR) -lglfw3 -lglew32 -lopengl32 -lIL -lfreetype -liconv

default: clear

	mkdir -p $(LIB_DIR)
	mkdir -p $(INCLUDE_DIR)
	
	$(CC) $(CFLAGS) -c src/ge.c
	ar rc $(LIB_DIR)/libge.a ge.o
	
	$(MAKE) -C examples
	
install:

	cp -rf include/ge.h /mingw64/include/ge.h
	cp -rf lib/libge.a /mingw64/lib/libge.a
	
clear:

	rm -f *.o
	rm -f $(LIB_DIR)/*.*
	rm -f $(TARGET)
	
	rm -f examples/*.o
	rm -f examples/*.exe