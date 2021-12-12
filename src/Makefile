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

#-b baker
#-t trasher
#-a acres of wheat
#-m mills
#-s shopkeepers

#-o output file
#-d simulation time [months]

exp1: all
	./$(OUTPUT) -a 10 -t 1 -m 1 -b 1 -d 12 -o exp1.dat

exp2: all
	./$(OUTPUT) -a 10 -t 1 -m 1 -b 5 -d 12 -o exp2.dat

clean: 
	rm $(OUTPUT) $(DAT) exp1.dat exp2.dat
