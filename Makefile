libcard:card.o deck.o
	g++ -g -o libcard card.o deck.o

card.o:card.cpp card.h
	g++ -g -o card.cpp

deck.o:deck.cpp deck.h
	g++ -g -o deck.cpp

clean:
	rm libcard card.o deck.o