#include <iostream>
#include <cmath>
#define NUM1 100000

void doit()
{
	double x=0;
	for(int i=0; i<NUM1; i++) x+=sin(i);
}

void f()
{
	for (int i=0; i<10000; i++) doit();
}

void g()
{
	for (int i=0; i<50000; i++) doit();
}

int main()
{
	double s = 0;
	for (int i=0; i<10000*NUM1; i++) s+=sqrt(i);
	f();
	g();
	std::cout <<"Done" << std::endl;
	exit(0);
}


