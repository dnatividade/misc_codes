/*
	Simple example of conversion: degree Celsius to Fahrenheit, Kelvin, Réaumur and Rankine
	
	Created/adapted: dnatividade
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc,char *argv[]){	//recebe o valor a ser convertido por parametro;
	float c, f, k, re, ra;		//variaveis para receber o valor em Celsius, Fahrenheit, Kelvin, Réaumur e Rankine;

	c =  atof(argv[1]);		//converte o argumento recebido (string) em inteiro;
	f =  1.8 * c + 32;		//conversao de oC para oF;
	k =  c + 273.15;		//conversao de oC para oK;
	re = 0.8 * c;			//conversao de oC para oRe
	ra = 1.8 * c + 32 + 459.67;	//conversao de oC para oRa

	printf("Celsius:    %f\n", c);		//Imprime na tela;
	printf("Fahrenheit: %f\n", f);
	printf("Kelvin:     %f\n", k);
	printf("Reuamur:    %f\n", re);
	printf("Rankine:    %f\n\n", ra);

	return(0);
}
