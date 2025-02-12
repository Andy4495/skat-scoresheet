# Skat Card Game Scoring Sheet
# Copyright 2025 Andreas Taylor
# https://github.com/Andy4495/skat-scoresheet
# MIT License

# 0.1  10-Feb-2025 Andy4495 Initial code


OBJS	= \
	skat_game.o \
	skat-scoresheet.o

OUT	= skat-scoresheet
CC	 = g++
FLAGS	 = -c -Wall -std=c++11
LFLAGS	 = 

all: skat-scoresheet

skat-scoresheet: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

debug: FLAGS += -DDEBUG -g --verbose
debug: LFAGS += --verbose
debug: all

verbose: FLAGS += --verbose
verbose: LFLAGS += --verbose
verbose: all

skat_game.o: skat_game.cpp skat_game.h
	$(CC) $(FLAGS) skat_game.cpp 

skat-scoresheet.o: skat-scoresheet.cpp skat_game.o
	$(CC) $(FLAGS) skat-scoresheet.cpp 

clean:
	rm -f $(OBJS) $(OUT)
