#
# Makefile
#
# The Makefile for libcard 
#
# huangtao117@gmail.com
#
CC=gcc

libcard:card.o deck.o
	ar -r libcard.a card.o deck.o

card.o:card.cpp card.h
	g++ -g -c card.cpp

deck.o:deck.cpp deck.h card.h
	g++ -g -c deck.cpp

clean:
	rm libcard card.o deck.o