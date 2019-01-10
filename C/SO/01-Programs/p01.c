//1)
// Fazer um programa que crie um processo que recebe o sinal SIGUSR1. No caso de recepcao de sinal escreve mensagem na tela. Programa principal apresenta seu PID e apos isso permanece dormindo.

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>


void handler(int sign) {
	printf("SIGNAL received (Diego e Lair): %d\n", sign);
	exit(0);
}


int main(void) {
	printf("PID: %d\n", getpid());
	sleep(1);
//	while (signal(SIGUSR1, handler) == 0)
//		sleep(1);

//	printf("Waiting signal..\n");
	signal(SIGUSR1, handler);
//	signal(SIGUSR2, handler);
//	printf("Signal received!\n");
	while(1)
		sleep(1);
} 

