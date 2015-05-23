CFLAGS = -I ./include
LIB    = ./libggfonts.so ./libfmodex64.so
LFLAGS = $(LIB) -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: zombie

zombie: zombiesarc.cpp fmod.c ppm.c log.c delete.cpp brandonW.cpp sounds.cpp alvaroJ.cpp 
	g++ $(CFLAGS) zombiesarc.cpp fmod.c ppm.c log.c delete.cpp brandonW.cpp sounds.cpp alvaroJ.cpp -Wall -Wextra $(LFLAGS) -o zombie

clean:
	rm -f zombie
	rm -f *.o

