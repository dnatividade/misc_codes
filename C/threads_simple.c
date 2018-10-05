/*
	Simple example to create many threads with pthread_create()
	
	Created/adapted: dnatividade
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//#define N_THREAD 11

void *THREAD_FUNCTION(void *tmp){
	int id;
	id = *(int*)tmp;

	printf("Thread [%d] is running.\n", id);
		sleep(5);		//aguarda por 5 segundos antes de terminar
		pthread_exit(NULL);	//termina a thread
	}


int main(int argc, char *argv[]){
	if (argc != 2) {
		printf("\nUsage: %s <number_of_threads>\n", argv[0]);
		printf("E.g.: %s 11\n\n", argv[0]);	
		return EXIT_FAILURE;
	}
	
	int N_THREAD = atoi(argv[1]);
	
	pthread_t threads[N_THREAD];
	int exec, i;

	for (i=0; i<N_THREAD; i++){
		printf("Creating thread [%d]\n", i);
		//chama a função que será executada pela thread
		exec = pthread_create(&threads[i], NULL, THREAD_FUNCTION, (void *)&i);
	}

	sleep(1);
	return(0);
}

