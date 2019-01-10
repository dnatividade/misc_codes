#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "frufru.h"
#include <time.h>

//#define N		10
#define LEFT		(i+N-1)%N
#define RIGHT		(i+1)%N
#define THINKING	0
#define HUNGRY		1
#define EATING		2

#define down		sem_wait
#define up			sem_post
#define TRUE		1

typedef sem_t semaphore;

//functions
void printPStatus(void);
void *philosopher(void *j);
void take_forks(int i);
void put_forks(int i);
void test(int i);
void think(void);
void eat(void);

int state[1000], i, nRand;
int N;
int T;

//semafores
semaphore mutex;
semaphore s[1000];


void banner(void) {
	printf("------\n");
	printf("A4-P01\n");
	printf("------\n\n");
	
	printf("--------------------\n");
	printf("Philosopher's Dinner\n");
	printf("--------------------\n\n");
	sleep(2);
}

//print philosopher status
void printPStatus(void){
	for (i=1; i<=N; i++){
		if(state[i-1] == THINKING){
			printf(ANSI_COLOR_CYAN "The philosopher [%d] is THINKING\n" ANSI_COLOR_RESET, i);
		}
		if(state[i-1] == HUNGRY){
			printf(ANSI_COLOR_YELLOW "The philosopher [%d] is HUNGRY\n" ANSI_COLOR_RESET, i);
		}
		if(state[i-1] == EATING){
			printf(ANSI_COLOR_RED "The philosopher [%d] is EATING\n" ANSI_COLOR_RESET, i);
		}
	}
	printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
}
 
//steps of philosopher (call all function)
void *philosopher(void *j){
	int i= *(int*) j;
	while (TRUE){  
		think();
		take_forks(i);
		eat();
		put_forks(i);
	}
}

//take the forks
void take_forks(int i){
	down(&mutex);
	state[i] = HUNGRY;
	printPStatus();
	test(i);
	up(&mutex);
	down(&s[i]);
}

//put the forks
void put_forks(int i){
	down(&mutex);
	state[i] = THINKING;
	printPStatus();
	test(LEFT);
	test(RIGHT);
	up(&mutex);
}
 
//it tests whether philosopher can eat
void test(int i){
	if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING){
		state[i] = EATING;
		printPStatus();
		up(&s[i]);
	} 
}
 
//phiosopher is thinking
//wait a random time (0 - 999999)
void think(void){
//	nRand = random() % 100000;
	nRand = random() % T+1;
	usleep(nRand);
}
 
//wait a random time (phiosopher is eating)
void eat(void){
//	nRand = random() % 100000;
	nRand = random() % T+1;
	usleep(nRand);
}
 

int main(int argc, char *argv[]){
	banner();

	if (argc != 3) {
		printf("\nUsage: %s <number_philosophers> <max_time_eat_think(microseconds)>\n", argv[0]);
		printf("E.g.: %s 10 5000000\n\n", argv[0]);	
		return EXIT_FAILURE;
	}

	N = atoi(argv[1]);
	T = atoi(argv[2]);
	
	pthread_t thread[N];
	int result;
	void *thread_result;
	int seed = time(NULL);
	
	//new seed to random numbers
	srand(seed);
	
	//initialize philosophers as THINKING
	for (i=0; i<N; i++){
		state[i] = THINKING;
	}
	printPStatus();

	//start semafores
	result = sem_init(&mutex, 0, 1);
	if (result != 0){
		perror("Semafore initialization error.");
		exit(EXIT_FAILURE);
	}
	for (i=0; i<N; i++){
		result = sem_init(&s[i], 0, 0);
		if (result != 0){
			  perror("Semafore initialization error.");
			  exit(EXIT_FAILURE);
		}
	}
 
	//create a thread to each philosopher
	for (i=0; i<N; i++){
		result = pthread_create(&thread[i], NULL, philosopher, &i);
		if (result != 0){
			perror("Thread initialization error.");
			exit(EXIT_FAILURE);
		}
	}

	//join the threads
	for (i=0; i<N; i++){
		result = pthread_join(thread[i], &thread_result);
		if (result != 0){
			perror("Thread join error.");
			exit(EXIT_FAILURE);
		}
	}
}

