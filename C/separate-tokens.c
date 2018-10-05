/*
	Simple example to separete words of a string using strtok()

	Created/adapted: dnatividade
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void main(){
	char str[500], *word;
	
	puts("Enter with the words: ");
	scanf("%[^\n]s", str);
	setbuf(stdin, NULL);
	word = strtok(str, " ");
	
	while(word != NULL) {
		printf("%s\n", word);
		word = strtok(NULL, " ");
	}
}

