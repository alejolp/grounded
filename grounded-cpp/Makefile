# Grounded Makefile.

CC=g++
CFLAGS=-c -Wall -O2 $(shell sdl2-config --cflags)
LDFLAGS=$(shell sdl2-config --libs)
SOURCES=grounded.cpp renderer.cpp game.cpp util.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=grounded

all: $(SOURCES) $(EXECUTABLE)

# :set noexpandtab

$(EXECUTABLE): $(OBJECTS) 
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(EXECUTABLE) $(OBJECTS)

