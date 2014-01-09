# Grounded Makefile.

CC=gcc
CFLAGS=-c -Wall -O2
LDFLAGS=-lSDL
SOURCES=Grounded.c Util.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=grounded

all: $(SOURCES) $(EXECUTABLE)

# :set noexpandtab

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

