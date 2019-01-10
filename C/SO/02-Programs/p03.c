//A2-03
//Fazer um programa que crie 10 filhos, cada filho espera um tempo randomico e acaba. O pai espera cada filho acabar e escreve na tela o numero do filho que acabou. Usando SIGCHILD.

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
	printf("A2-P03\n");
	printf("------\n\n");
	
	printf("-----------------------------------\n");
	printf("Random time to die (using SIGCHILD)\n");
	printf("-----------------------------------\n");
}


void handler(int sig) {
	pid_t pid;
	pid = wait(NULL);
	printf("Pid %d exited.\n", pid);
}


int main(void) {
	//Print banner//
	banner();

	int pid[N];
	
	signal(SIGCHLD, handler);
//	signal(SIGUSR1, handler);

	printf("[.]Creating children...\n");
	for (int ii=0; ii<N; ii++) {
		if (pid[ii] = fork()) {	
			//////////
			//FATHER//
			//////////
			printf("%d created.\n",  pid[ii]);
			
			if (ii == N-1)
				printf("[.]Waiing for children process to die...\n");
		}		
		else {
			////////////
			//CHILDREN//
			////////////
			//while(!signal_flag) {
			////wait until signal_flag = TRUE//
			//}
			srand(getpid());
			int T;
			T = rand() % 11;
			sleep(T);
			printf("%d dead.\n", getpid());
			//_exit(EXIT_SUCCESS);
			exit(0);
		}
	}

	//wait(NULL);
	for (int ii=0; ii<N; ii++) {
		wait(NULL);
	//	pause();
	}
	printf("\n\nAll the children process are dead.\n");
	printf("Application finished!\n");
	
	return 0;
}



//signal(SIGCHLD, SIG_IGN);

