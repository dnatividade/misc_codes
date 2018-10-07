/*
	Simple example of malloc() use with array
	
	Created/adapted: dnatividade
*/

#include <stdio.h>
#include <stdlib.h>

int main(void) {
	const int LOOP = 10;
	int j;
	long *a;
	//long n = 1000000000;	//1Bi
	long n = 700000000;	//1Bi
	long i=0;

	printf("ARRAY LENGTH:  %ld\n", n);
	printf("Operation:     a[i] = i\n");


	for (j=0; j<LOOP; j++){
	a = (long *) malloc(n * sizeof(long));
	if (a == NULL){
		printf("\nError: there is not memory\n");
		exit(1);
	}

	for (i=0; i<n; i++){
		a[i]=i;
		//printf("a[%d]: %d\n",i,a[i]);
	}

	printf("LOOP %d, a[%ld]: %ld\n", j, i, a[i]);
	free(a);
	}

	exit(0);
}
