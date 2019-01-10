//2)
//Fazer um programa que crie 2 processos filhos. O processo pai deve enviar uma mensagem para o primeiro filho. A mesma mensagem deve ser enviada do primeiro filho para o segundo e o segundo escreve na tela.

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
	int pipefd1[2], pipefd2[2];
	pid_t pid1, pid2;
	char  buf1, buf2;

	//Check argument numbers
	if (argc != 2) {
		//fprintf(stderr, "Usage: %s <string>\n", argv[0]);
		printf("Arguments number wrong!\n");
		printf("Usage: %s <string>\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	//Check pipe error
	printf("[.]Creating pipe 1...\n");
	if (pipe(pipefd1) == -1) {
		printf("[x]Pipe 1 error.\n");
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	printf("[.]Creating pipe 2...\n");
	if (pipe(pipefd2) == -1) {
		printf("[x]Pipe 2 error.\n");
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	//Create child 1
	printf("[.]Creating child 1...\n");
	pid1 = fork();
	
	//Check fork() error
	if (pid1 == -1) {
		printf("[x]Fork error (child 1).\n");
		perror("fork");
		exit(EXIT_FAILURE);
	}

	//Is child reading pipe?
	if (pid1 == 0) {		/* Child reads from pipe */
		close(pipefd1[1]);	/* Close unused write end */

		//printf("Child 1: ");
		write(STDOUT_FILENO, "Child 1: ", 10);

		while (read(pipefd1[0], &buf1, 1) > 0) {
			write(STDOUT_FILENO, &buf1, 1);
		        write(pipefd2[1], &buf1, strlen(&buf1));
		}
		write(STDOUT_FILENO, "\n", 1);
		close(pipefd1[0]);
		
	        close(pipefd2[0]);	/* Close unused read end */
	        close(pipefd2[1]);	/* Reader will see EOF */
	        		
		_exit(EXIT_SUCCESS);


	//Is father reading pipe?
	} else {			/* Parent writes argv[1] to pipe */
	        close(pipefd1[0]);	/* Close unused read end */
	        write(pipefd1[1], argv[1], strlen(argv[1]));
	        close(pipefd1[1]);	/* Reader will see EOF */

		printf("[.]Creating child 2...\n");
		pid2 = fork();
		if (pid2 == 0) {		/* Child reads from pipe */
			close(pipefd2[1]);	/* Close unused write end */
	
			printf("Child 2: ");
			//sleep(1);
			while (read(pipefd2[0], &buf2, 1) > 0)
				write(STDOUT_FILENO, &buf2, 1);
	
			write(STDOUT_FILENO, "\n", 1);
			close(pipefd2[0]);
			_exit(EXIT_SUCCESS);
	
		        
		        wait(NULL);		/* Wait for child */
		        exit(EXIT_SUCCESS);
		}
	}

}
