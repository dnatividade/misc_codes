//3)
//Criar um programa que recebe como paramentro o número de filhos "n" e também outro parâmetro "m". Cada processo filho terá a tarefa de calcular uma aproximação de PI, utillizando o método de Monte Carlo, com sorteio de "m" pontos.
//Após isso, mandará o PI calculado para o processo pai. O processo pai irá fazer uma média e apresentar na tela. O processo filho antes de iniciar o processamento, espera o sinal do processo pai. O pai mandará o sinal para os "n" filhos e após isso, receberá os resultados por PIPES.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>

#define USECOND 1
#define SQUARE_AREA(l) ((l*l))

bool signal_flag = false;

void banner(void) {
	printf("------\n");
	printf("A1-P03\n");
	printf("------\n\n");
	
	printf("-----------------------------------------------------------\n");
	printf("Calculation of Pi by approximation using Monte Carlo method\n");
	printf("-----------------------------------------------------------\n");
}


unsigned long int monteCarlo(unsigned long int m) {
	float x, y;
	unsigned long int result = 0;
	
	for (unsigned long int i=0; i<m; i++) {
		x = (float)rand()/RAND_MAX*2;
		y = (float)rand()/RAND_MAX*2;
	
		// X^2 + Y^2 < R^2 //
		if ( (1-x)*(1-x) + (1-y)*(1-y) < 1 )
			result++;
	}
	
	return result;
}


void handler(int sign) {
	signal_flag = true;
}

int main(int argc, char *argv[]) {
	//Print banner//
	banner();

	if (argc != 3) {
		printf("\nUsage: %s <number_of_children> <number_of_coord>\n", argv[0]);
		printf("E.g.: 50 children and 500,000 coordenates (x,y)\n\n", argv[0]);	
		printf("%s 50 500000\n\n", argv[0]);	
		return EXIT_FAILURE;
	}

	int n;
	unsigned long int m, prop;
	float pi = 0;
	float buf;
	float avg = 0;
	n = atoi(argv[1]);
	m = atol(argv[2]);
	int pid[n];
	int pipefd[n][2];	

	printf("-------------------------------------------\n");
	printf("Number of children     = %d\n",  n);
	printf("Number of coord. (x,y) = %lu\n", m);
	printf("-------------------------------------------\n\n");

	signal(SIGUSR1, handler);

	//Create pipes and check for errors //////////////////////
	printf("[.]Creating pipes...");
	for (int ii=0; ii<n; ii++) {
		if (pipe(pipefd[ii]) == -1) {
			printf("[x]Pipe error.\n");
			perror("pipe");
			exit(EXIT_FAILURE);
		}
	}
	printf("     OK!\n");
	//////////////////////////////////////////////////////////

	printf("[.]Creating children...");
	for (int ii=0; ii<n; ii++) {
		if (pid[ii] = fork()) {
			//////////
			//FATHER//
			//////////
			if (ii == n-1)
				printf("  OK!\n\n");
		}else {
			////////////
			//CHILDREN//
			////////////
			while(!signal_flag) {
				//wait until signal_flag = TRUE//
				usleep(USECOND);
			}
			srand(getpid());
			prop = monteCarlo(m);
			pi = (float)prop/m*SQUARE_AREA(2);

	        	printf("pi: %f\n", pi);
			
	        	write(pipefd[ii][1], &pi, sizeof(float));

			_exit(EXIT_SUCCESS);
		}
	}

	for (int ii=0; ii<n; ii++) {
		kill(pid[ii], SIGUSR1);
		wait(NULL);
		read(pipefd[ii][0], &buf, sizeof(float));
		avg = avg + buf;
	}

	avg = avg/n;
	printf("\n\nPID %i says: PI AVG ~ %f\n", getpid(), avg);
	
	return 0;
}



