CC=g++

IDIR = ../include
INCLUDES  = -I.
INCLUDES += -I$(IDIR)
# INCLUDES += -I/usr/include/SDL2 -D_REENTRANT
CFLAGS=-g -std=c++20 -D_REENTRANT $(INCLUDES)

SDL=sdl2-config
SDLFLAGS=--cflags --libs

ODIR=obj
LDIR =../lib

LIBS=-lm -lSDL2

_DEPS = chip8.hpp util.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o chip8.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

chip8: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -rf obj/* chip8

run:
	./chip8
