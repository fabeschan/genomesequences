CXX=clang++
CXXFLAGS=-g -std=c++11 -Wall -pedantic

all: out

out: avltree.o main.o
	g++ main.o avltree.o -o out

main.o: main.cpp
	g++ -c main.cpp

avltree.o: avltree.cpp
	g++ -c avltree.cpp

clean:
	rm -rf *o out
