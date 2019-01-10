//A2-01
//Criar um programa que sorteie 50 numeros e envie-os para o programa 2 atravez de uma FIFO.
// OU
//Fazer um chat em que 2 processos se comuniquem via FIFO

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <string.h>
#include <unistd.h>

#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (x), (y))

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


size_t LEN = 100;

void banner(void) {

	printf("------\n");
	printf("A2-P01\n");
	printf("------\n\n");
	
	printf(ANSI_COLOR_GREEN  "-------------------------------------\n" ANSI_COLOR_RESET);
	printf(ANSI_COLOR_YELLOW "CIP - Chat Inter Process (using FIFO)\n" ANSI_COLOR_RESET);
	printf(ANSI_COLOR_GREEN  "-------------------------------------\n" ANSI_COLOR_RESET);
}


void removeLineFeed() {
	
}

int main(int argc, char *argv[]) {
	int fd, pidC;
	char MY_FNAME[15];
	char REMOTE_FNAME[15];
	char str[LEN];
	char *msg;
	
	clear();
	banner();
	
	if (argc != 3) {
		printf("\nUsage: %s <my_FIFO_name> <remote_FIFO_name>\n\n", argv[0]);
		printf("E.g.:\n");
		printf("To fist chat:\n");	
		printf("%s FIFO1 FIFO2\n", argv[0]);	
		printf("To second chat:\n");	
		printf("%s FIFO2 FIFO1\n\n", argv[0]);	
		return EXIT_FAILURE;
	}

	strcpy(MY_FNAME, argv[1]);
	strcpy(REMOTE_FNAME, argv[2]);
	printf(ANSI_COLOR_GREEN "MY_FNAME = "ANSI_COLOR_BLUE"%s\n" ANSI_COLOR_RESET, MY_FNAME);
	printf(ANSI_COLOR_GREEN "REMOTE_FNAME = "ANSI_COLOR_BLUE"%s\n" ANSI_COLOR_RESET, REMOTE_FNAME);
	printf(ANSI_COLOR_GREEN "-------------------------------------\n" ANSI_COLOR_RESET);
	printf("\n");

	mkfifo(MY_FNAME, 0660);
	mkfifo(REMOTE_FNAME, 0660);

	pidC = fork();

	while(1) {
		/////////
		//CHILD//
		/////////
		if (pidC == 0) {
			fd = open(REMOTE_FNAME, O_RDONLY);	//open REMOTE FIFO
			if (fd<0) {
				printf("Error to open FIFO.\n");
				exit(1);
			}
			
			if (read(fd, str, LEN) > 0) {
				//printf("Remote: %s", str);
				printf(ANSI_COLOR_RED "Remote: " ANSI_COLOR_RESET);
				printf("%s", str);
			}
			close(fd);				//close REMOTE FIFO
		}
		//////////
		//FATHER//
		//////////
		else {
			printf(ANSI_COLOR_RESET);
			do {
				fd = open(MY_FNAME, O_WRONLY);	//open MY FIFO
				if (fd==-1)
					sleep(1);
			}while (fd==-1);
				
			msg = (char*)malloc(LEN*sizeof(char));
			printf(ANSI_COLOR_RESET);
			getline(&msg, &LEN, stdin);
			//printf(ANSI_COLOR_RESET);
			write(fd, msg, strlen(msg) + 1);
			sleep(0.1);
			close(fd);				//close MY FIFO
			free(msg);
		}
	}	
}





