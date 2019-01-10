//A2-04
//Fazer um pequeno interpretador que espere o usuario digitar um comando, execute-o e retorne para que o usuario digitar o proximo comando. usar FORK e EXECVE.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>


#define clear() printf("\033[H\033[J")

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//#define LEN 50
size_t LEN = 256;
char *dir;


void banner(void) {
	printf("------\n");
	printf("A2-P04\n");
	printf("------\n\n");
	
	printf(ANSI_COLOR_GREEN "-------------------\n" ANSI_COLOR_RESET);
	printf(ANSI_COLOR_CYAN  "Command Interpreter\n" ANSI_COLOR_RESET);
	printf(ANSI_COLOR_GREEN "-------------------\n" ANSI_COLOR_RESET);
	printf(ANSI_COLOR_RESET "\n" ANSI_COLOR_RESET);
}


void prompt() {
	dir = getcwd(NULL, LEN);
	printf(ANSI_COLOR_RESET "(" ANSI_COLOR_RESET);
	printf(ANSI_COLOR_BLUE  "%s" ANSI_COLOR_RESET, dir);
	printf(ANSI_COLOR_RESET ")" ANSI_COLOR_RESET);
	//
	printf(ANSI_COLOR_CYAN  "$" ANSI_COLOR_RESET);
	printf(ANSI_COLOR_RESET "_ " ANSI_COLOR_RESET);
}


int main(void) {
	char cmd[2][LEN];
	char *args[2][LEN];
	char *token;
	int countTk=0;
	int countPp=0;
	bool pipeOk=false;

	int fd[2];


	clear();
	banner();

	int pid;
	char str[LEN];

	while (true) {
		
		//signal(SIGCHLD, SIG_IGN);
		prompt();
		
		//Create pipe
		if (pipe(fd) < 0) {
			perror("pipe") ;
			return -1;
		}

		
		setbuf(stdin, NULL);
		scanf("%[^\n]s", str);
		setbuf(stdin, NULL);
		//fgets(cmd, sizeof(cmd), stdin);

		//temporary vars and procedures //////////////
		char *argsTmp[LEN];
		char *tkTmp;
		char strTmp[LEN];
			
		strcpy(strTmp, str);
		
		tkTmp = strtok(strTmp, " ");
		argsTmp[0] = tkTmp;
		tkTmp = strtok (NULL, " ");
		argsTmp[1] = tkTmp;
		//////////////////////////////////////////////
		
		if (!strcmp(str, "exit")) break;
		else if (!strcmp(argsTmp[0], "cd")) {
			chdir(argsTmp[1]);
		}

		//printf("\n\n");
		countTk = 0;
		token = strtok(str, " ");
		while (token != NULL) {
			if (countTk == 0)
				strcpy(cmd[0], token);
			args[0][countTk] = token;
			
			if (*args[0][countTk] == '|') {
				pipeOk = true;
				args[0][countTk] = NULL;
				
				countPp = 0;
				token = strtok(NULL, " ");	
				while (token != NULL) {
					if ((countPp) == 0)
						strcpy(cmd[1], token);
					args[1][countPp] = token;
					token = strtok (NULL, " ");
					//printf("%s\n", args[0][countTk]);
					countPp++;
				}
				args[1][countPp] = NULL;
				break;			
			}
			token = strtok (NULL, " ");
			countTk++;
		}
		
		if (pipeOk == false) {
			args[0][countTk] = NULL;

			// ONLY 1 CHILD //
			pid = fork();
			if (pid == 0) {
				execvp(cmd[0], args[0]);
				_exit(EXIT_FAILURE);
			}			
		}
		else {
			pipeOk = false;
			//Create pipe
			if (pipe(fd) < 0) {
				perror("pipe") ;
				return -1;
			}

			// CHILD 1 //
			pid = fork();
			if (pid == 0) {
				close(fd[0]);
				dup2(fd[1], 1);
				//
				execvp(cmd[0], args[0]);
				_exit(EXIT_FAILURE);
			}

			// CHILD 2 //
			pid = fork();
			if (pid == 0) {
				close(fd[1]);
				dup2(fd[0], 0);
				//
				execvp(cmd[1], args[1]);
				_exit(EXIT_FAILURE);
			}
			close(fd[0]);
			close(fd[1]);
		}
		wait(NULL);
		wait(NULL);
		
		//printf("\n\n");
	}
		
	printf("\nCommand Interpreter has been finished!\n");
	return 0;
}



