/* 
* Compile: gcc p01.c -o p01 -pthread
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#include "frufru.h"

#define DATA_MAX_VALUE	100	//max number generate by random function (from 0 to DATA_MAX_VALUE)
#define QUEUE_LEN	20	//queue lenght
#define TRUE		1


void banner(void) {
	printf("------\n");
	printf("A4-P01\n");
	printf("------\n\n");
	
	printf("-------------------\n");
	printf("Producer & Consumer\n");
	printf("-------------------\n\n");
}


void producer(int *idx, sem_t *full, sem_t *empty, sem_t *mutex, int *dataRegion) {
	int i=0, data=0;
	*idx=0;
	while(TRUE) {
		data = rand() % (DATA_MAX_VALUE+1);
		sem_wait(empty);
		sem_wait(mutex);
		
		if (i < QUEUE_LEN) {
			dataRegion[i] = data;
			//*(dataRegion + i) = data;
			printf("[%d] Data: %d\n", i, data);
			fflush(stdout);
			i++;
		}else
			i=0;
		
		sem_post(mutex);
		sem_post(full);
			
		sleep(1);
	}
}


void consumer(int *idx, sem_t *full, sem_t *empty, sem_t *mutex, int *dataRegion) {
	int i=0, data=0;
	while(TRUE) {
		sem_wait(full);
		sem_wait(mutex);
		
		if (i < QUEUE_LEN) {
			data = dataRegion[i];
			//*(dataRegion + i) = data;
			printf("[%d] Data: %d\n", i, data);
			fflush(stdout);
			i++;
		}else
			i=0;
		
		sem_post(mutex);
		sem_post(empty);
			
		sleep(1);
	}
}


int main(int argc, char *argv[]) {

	banner();
	if (argc != 2) {
		printf("\nUsage: %s <p|c>\n", argv[0]);
		printf("To run producer, use: %s p\n", argv[0]);
		printf("To run consumer, use: %s c\n\n", argv[0]);
		return EXIT_FAILURE;
	}

	int sizeInt = sizeof(int);
	int sizeSemaphore = sizeof(sem_t);
	int memLen = sizeInt + (QUEUE_LEN * sizeInt) + (3*sizeSemaphore);
	char createFile[55];
	char *createFileTmp = "dd if=/dev/zero of=prod_cons.mem bs=1 count=%d";
	
	int fd;
	int *idx;
	sem_t *full;
	sem_t *empty;
	sem_t *mutex;
	int *dataRegion;
	char *mem;

	char *option = argv[1];
	switch (*option) {
		case 'p':
			printf(ANSI_COLOR_RESET   "-------------------\n");
			printf(ANSI_BGCOLOR_CYAN_L ANSI_COLOR_BLACK "Producer running...\n" ANSI_COLOR_RESET);
			printf(ANSI_COLOR_RESET   "-------------------\n\n");

			srand(time(NULL));

			sprintf(createFile, createFileTmp, memLen);

			printf("---------------------------\n");
			printf("CONFIGURATION:\n");
			printf("Size of int: %d\n", sizeInt);
			printf("Size of sem_t: %d\n", sizeSemaphore);
			printf("MemLen: %d\n", memLen);
			system(createFile);
			printf("---------------------------\n\n", sizeSemaphore);
			
			fd = open("prod_cons.mem", O_RDWR);
			mem = mmap(NULL, memLen, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
			
			/*
			idx        = (int*)mem;
			full       = (sem_t*)mem + sizeInt;
			empty      = (sem_t*)(mem + sizeInt + sizeSemaphore);
			mutex      = (sem_t*)(mem + sizeInt + 2*sizeSemaphore);
			dataRegion = (int*)(mem + sizeInt + 3*sizeSemaphore);
			*/
			full       = (sem_t*)mem;
			empty      = (sem_t*)(mem + sizeSemaphore);
			mutex      = (sem_t*)(mem + 2*sizeSemaphore);
			dataRegion = (int*)(mem + 3*sizeSemaphore);
			idx        = (int*)(mem + 3*sizeSemaphore + QUEUE_LEN*sizeInt);
			
			sem_init(full,  1, 0);
			sem_init(empty, 1, QUEUE_LEN);
			sem_init(mutex, 1, 1);

			producer(idx, full, empty, mutex, dataRegion);
			
			
		case 'c':
			printf(ANSI_COLOR_RESET   "-------------------\n");
			printf(ANSI_BGCOLOR_MAGENTA_L ANSI_COLOR_RED "Consumer running...\n" ANSI_COLOR_RESET);
			printf(ANSI_COLOR_RESET   "-------------------\n\n");

			fd = open("prod_cons.mem", O_RDWR);
			mem = mmap(NULL, memLen, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
			
			/*
			idx        = (int*)mem;
			full       = (sem_t*)mem + sizeInt;
			empty      = (sem_t*)(mem + sizeInt + sizeSemaphore);
			mutex      = (sem_t*)(mem + sizeInt + 2*sizeSemaphore);
			dataRegion = (int*)(mem + sizeInt + 3*sizeSemaphore);
			*/
			full       = (sem_t*)mem;
			empty      = (sem_t*)(mem + sizeSemaphore);
			mutex      = (sem_t*)(mem + 2*sizeSemaphore);
			dataRegion = (int*)(mem + 3*sizeSemaphore);
			idx        = (int*)(mem + 3*sizeSemaphore + QUEUE_LEN*sizeInt);
			
			consumer(idx, full, empty, mutex, dataRegion);
		
		
		default:
			printf("Invalid option!\n");
			printf("\nUsage: %s <p|c>\n", argv[0]);
			printf("To run producer, use: %s p\n", argv[0]);
			printf("To run consumer, use: %s c\n\n", argv[0]);
			return EXIT_FAILURE;
		}
	
	return(0);
}


