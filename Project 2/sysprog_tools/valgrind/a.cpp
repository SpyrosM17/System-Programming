#include <iostream>

char *f() { char *cp = new char[17]; return cp; }

#define MM 100000

int main()
{
	int *p = new int[10];
	p[10] = 6;

	int i,j;
	j = i+3;
	if (i > 0) std::cout << "Hi"; 

	f();
	free(p);
	return 0;
}
