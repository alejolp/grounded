# Grounded Makefile.

CC=gcc
CFLAGS=-c -Wall -O2
LDFLAGS=-lSDL
SOURCES=Grounded.c Util.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=grounded

all: $(SOURCES) $(EXECUTABLE)

# :set noexpandtab

$(EXECUTABLE): $(OBJECTS) 
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(EXECUTABLE) $(OBJECTS)

