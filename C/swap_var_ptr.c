// Simple example to change values between 2 variables using pointers

#include <stdio.h>
#include <stdlib.h>

void swap(int* x, int* y){
	int tmp;
	tmp= *x;
	*x= *y;
	*y= tmp;
}

int main(void){
	int x,y;
	printf("Enter X: ");
	scanf("%d",&x);

	printf("Enter Y: ");
	scanf("%d",&y);

	swap(&x,&y);

	printf("New X value: %d\n", x);
	printf("New Y value: %d\n", y);

	return 0
}

