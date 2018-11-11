// Simple example using memset()
// Exemplo simples de uso do memset()

#include <stdio.h> 
#include <string.h> 

void showVector(int vector[], int n) { 
	for (int i=0; i<n; i++) 
		printf("%d ", vector[i]);
	printf("\n\n");
} 

int main() { 
	int n = 2; 
	int vector[n]; 
	//
	printf("int vector[%d], memset() 0\nInt: [00000000][00000000][00000000][00000000] = 0 \n", n); 
	memset(vector, 0, n*sizeof(*vector)); 
	showVector(vector, n); 
	//
	printf("int vector[%d], memset() 3\nInt: [00000011][00000011][00000011][00000011] = 50529027\n", n); 
	memset(vector, 3, n*sizeof(*vector)); 
	showVector(vector, n); 

	return 0; 
} 

