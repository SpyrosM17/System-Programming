CC=gcc
CFLAGS=-std=c99 -Wall -O3 -D_XOPEN_SOURCE -g
CXX=g++
CXXFLAGS=-O0

all: mmul mmul_gp example

mmul: mmul.c
	$(CC) $(CFLAGS) -o mmul mmul.c

mmul_gp: mmul_gp.c
	$(CC) $(CFLAGS) -pg -o mmul_gp mmul_gp.c
	# ./mmul_gp 1024
	# gprof -b mmul_gp gmon.out

example: example.cpp
	$(CXX) $(CXXFLAGS) -pg -o example example.cpp

clean:
	rm -f mmul mmul_gp example

