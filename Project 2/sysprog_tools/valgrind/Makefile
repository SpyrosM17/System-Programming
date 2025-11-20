CC=gcc
CFLAGS=-O3 -g
CXX=g++
CXXFLAGS=-O3 -g -ggdb

all: a.out mmul

a.out: a.cpp
	$(CXX) $(CXXFLAGS) -o a.out a.cpp

run_memcheck:
	valgrind --tool=memcheck --leak-check=yes ./a.out >& log.txt

mmul: mmul.c
	$(CC) $(CFLAGS) -o mmul mmul.c

clean:
	rm -f a.out mmul

