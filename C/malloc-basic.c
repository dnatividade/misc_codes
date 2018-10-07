/*
	Simple example of malloc() use
	
	Created/adapted: dnatividade
*/

#include <stdio.h>
#include <stdlib.h>

int main(void) {
	int *ptr;
	int i = 99;

	ptr = (int*)malloc(sizeof(int));
	if (ptr == NULL){
		printf("Error: there is not memory\n");
		exit(1);
	}

	*ptr = i;
	printf("*ptr: %d\n", *ptr);
	
	free(ptr);
	return(0);
}
