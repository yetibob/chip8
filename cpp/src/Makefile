FILENAME = chip8
OUTDIR = bin

CC = g++

# AVR-GCC Compilation Options
CFLAGS = -g -Os -c -o

# AVR-GCC Linking flags
LFLAGS = -Os -o

SRC = $(FILENAME).cpp

# Source files to compile
SRC +=

INCLUDE = -I.

# User defined and 3rd party libs to include
INCLUDE +=

$(FILENAME): $(FILENAME).o
	$(CC) $(INCLUDE) $(LFLAGS) $(OUTDIR)/$@ $(OUTDIR)/$^

$(FILENAME).o: $(SRC)
	$(CC) $^ $(INCLUDE) $(CFLAGS) $(OUTDIR)/$@


clean:
	rm -rf bin

bin:
	mkdir -p bin
