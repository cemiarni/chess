
CC=g++
CFLAGS=-O2
ALLEGFLAGS=`allegro-config --libs --cflags`

default:
	$(CC) -o sakk main.cpp sakk.cpp $(CFLAGS) $(ALLEGFLAGS)

debug:
	$(CC) -o sakk main.cpp sakk.cpp $(CFLAGS) -g -DDEBUG $(ALLEGFLAGS)
	
clean:
	rm -f ./sakk