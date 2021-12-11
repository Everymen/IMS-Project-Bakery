.PHONY: all run
CC = g++
FILE = main.cpp
SIMLIB = -lsimlib -lm
SW = -g -O2 -o
OUTPUT = bread
DAT = bread.dat

all: main.cpp
	$(CC) $(SW) $(OUTPUT) $(FILE) $(SIMLIB)


run: all
	./$(OUTPUT)

clean: 
	rm $(OUTPUT) $(DAT)
