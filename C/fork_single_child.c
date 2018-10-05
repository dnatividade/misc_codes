/*
	Simple example to create a child with fork()
	
	Created/adapted: dnatividade
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(void) {
	int pid = fork();	//cria filho
	
	if (pid != 0) {
		//este bloco será excutado somente pelo processo PAI
		printf("Father process (pid=%d)\n", getpid());
		wait(NULL);
	}else {
		//este bloco será excutado somente pelo processo FILHO
		printf("Child process (pid=%d)\n\n", getpid());
		_exit(0);
	}

	return(0);
}

