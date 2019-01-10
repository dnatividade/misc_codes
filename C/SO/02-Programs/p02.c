//A2-02
//Fazer um programa que crie 10 filhos, cada filho espera um tempo randomico e acaba. O pai espera cada filho acabar e escreve na tela o numero do filho que acabou.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>

#define N 10

void banner(void) {
	printf("------\n");
	printf("A2-P02\n");
	printf("------\n\n");
	
	printf("------------------\n");
	printf("Random time to die\n");
	printf("------------------\n");
}


int main(void) {
	banner();
	int pid[N];

	printf("[.]Creating children...\n");
	for (int ii=0; ii<N; ii++) {
		if (pid[ii] = fork()) {	
			//////////
			//FATHER//
			//////////
			printf("%d created.\n",  pid[ii]);
			
			if (ii == N-1)
				printf("[.]Waiting for children process to die...\n");
			//wait(NULL);
		}else {
			////////////
			//CHILDREN//
			////////////
			srand(getpid());
			int T;
			T = rand() % 11;
			sleep(T);
			printf("%d dead.\n", getpid());
			_exit(EXIT_SUCCESS);
		}
	}

	for (int ii=0; ii<N;ii++) {
		wait(NULL);
	}
	
	printf("\n\nAll the children process are dead.\n");
	printf("Application finished!\n");
	
	return 0;
}



