/* Fibonacci: sequential, version 1*/
#include <stdio.h>

int fib(int n)
{
	if (n <= 1) return n;
	else return fib(n-1) + fib(n-2);
}


int main(int argc, char * argv[])
{
    int n = 10;
    int res;

    res = fib(n);
    printf("fib(%d)=%d\n", n, res);

	return 0;
}
