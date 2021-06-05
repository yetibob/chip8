CC=g++

IDIR =../include
CFLAGS=-std=c++17 -I$(IDIR)

ODIR=obj
LDIR =../lib

LIBS=-lm

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
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
	rm -f $(BDIR)/* *~ core $(INCDIR)/*~ 

run:
	./chip8
