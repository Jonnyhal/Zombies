CFLAGS = -I ./include
LIB    = ./libggfonts.so
LFLAGS = $(LIB) -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: zombie

<<<<<<< HEAD
zombie: zombiesarc.cpp ppm.c log.c delete.cpp brandonW.cpp alvaroJ.cpp 
	g++ $(CFLAGS) zombiesarc.cpp ppm.c log.c delete.cpp brandonW.cpp alvaroJ.cpp -Wall -Wextra $(LFLAGS) -o zombie
=======
zombie: zombiesarc.cpp ppm.c log.c delete.cpp brandonW.cpp
	g++ $(CFLAGS) zombiesarc.cpp ppm.c log.c delete.cpp brandonW.cpp -Wall -Wextra $(LFLAGS) -o zombie
>>>>>>> 483e030ee4a8311d08ffb30168ce823966077952

clean:
	rm -f zombie
	rm -f *.o

