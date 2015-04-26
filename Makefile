CFLAGS = -I ./include
LIB    = ./libggfonts.so
LFLAGS = $(LIB) -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: zombie

zombie: zombiesarc.cpp ppm.c log.c
	g++ $(CFLAGS) zombiesarc.cpp ppm.c log.c -Wall -Wextra $(LFLAGS) -o zombie

clean:
	rm -f zombie
	rm -f *.o

