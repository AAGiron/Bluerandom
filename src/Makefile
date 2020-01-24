##Makefile for Bluerandom

all: defaultRule

defaultRule: bluerandom.o BLEScan.o scan.o extraction.o
	gcc -o bluerandom bluerandom.o BLEScan.o scan.o extraction.o -lbluetooth -lm

BLEScan: BLEScan.c
	gcc -o BLEScan.o -c BLEScan.c -W -Wall

scan: scan.c
	gcc -o scan.o -c scan.c -W -Wall

extraction: extraction.c
	gcc -o extraction.o -c extraction.c -W -Wall

bluerandom.o: bluerandom.c
	gcc -o bluerandom.o -c bluerandom.c -W -Wall 

clean:
	rm -rf bluerandom *.o




