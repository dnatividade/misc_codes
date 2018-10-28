// Example of pointer to function //

#include <stdio.h>

int sum(int a, int b) {
	return (a+b);
}

int mult(int a, int b) {
	return (a*b);
}


int main(void) {
	int a, b, result = 0;
	int (*operation)(int, int);

	a = 5;
	b = 7;

	printf("a= %d; b= %d\n", a, b);

	operation = &sum;
	result = (*operation)(a,b);
	printf("SUM: %d\n", result);

	operation = &mult;
	result = (*operation)(a,b);
	printf("MULT: %d\n", result);

	return 0;
}
