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
	
clear:

	rm -f *.o
	rm -f $(LIB_DIR)/*.*
	rm -f $(TARGET)
	
	rm -f examples/*.o
	rm -f examples/*.exe