/*
	Simple example of conversion: STRING to ASCII and HEX
	
	Created/adapted: dnatividade
*/

#include <stdio.h>

int main(int argc, char *argv[]) {
	char *str = argv[1];

	printf("CHAR - ASCII - HEX\n");
	while(*str) {
		printf("%c      %u      %x\n", *str, *str, *str);
		str++;
	}

	return(0);
}
