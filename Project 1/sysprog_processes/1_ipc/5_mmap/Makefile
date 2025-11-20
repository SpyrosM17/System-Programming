all: mytest

mytest: mytest.c mmap.c
	gcc -Wall -std=c99  -o mytest mytest.c mmap.c -lm

clean:
	rm -f mytest
	rm -rf *.dSYM

.PHONY: clean
