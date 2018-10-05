/*
	Simple example to create several children process using fork()
	
	Created/adapted: dnatividade
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//#define N_CHILDREN	5

int main(int argc, char *argv[]) {
	if (argc != 1) {
		printf("\nUsage: %s <number_of_children>\n", argv[0]);
		printf("E.g.: %s 17\n\n", argv[0]);	
		return EXIT_FAILURE;
	}
	
	int N_CHILDREN = atoi(argv[1]);
	int pid, pidFather = getpid();

	for (int i=0; i<N_CHILDREN; i++) {
		if (getpid() == pidFather) {
			//processo pai
			if (i == 0)
				printf("Father process (pid=%d)\n", pidFather);
			pid = fork();
		}
			
		if (pid == 0) {
			//processo filho
			printf("[%d] Child process (pid=%d)\n", i, getpid());
			//while(1);
			_exit(0);
		}
	}

	//aguarda até que todos os filhos terminem
	for (int i=0; i<N_CHILDREN; i++)
		wait(NULL);

	return(0);
}

