//Chap.1 (33)

////////////////
/* TCP Server */
////////////////

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <pthread.h>
#include "frufru.h"

#define MAX_PENDING 5
#define MAX_LINE 256

pthread_t th[MAX_PENDING];
int exec, i = 0;
struct sockaddr_in sockin;
char buf[MAX_LINE];
int len;
int SERVER_PORT;


void *THREAD(void *tmp) {
	int new_s_th[i];
	new_s_th[i] = *((int*)tmp);
	
	while (len = recv(new_s_th[i], buf, sizeof(buf), 0)) {
		fputs(inet_ntoa(sockin.sin_addr), stdout);
		fputs("\n", stdout);
		fputs("<- ", stdout);
		fputs(buf, stdout);
	}

	printf(ANSI_COLOR_MAGENTA "Client %s disconnected from port %d\n\n" ANSI_COLOR_RESET,
		inet_ntoa(sockin.sin_addr), ntohs(sockin.sin_port));
		
	/* close socket of received connection */
	close(new_s_th[i]);
	pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
	int s, new_s[MAX_PENDING];
	
	if (argc==2) {
		SERVER_PORT = atoi(argv[1]);
	}
	else {
		fprintf(stderr, "Usage: %s <port>\n\n", argv[0]);
		exit(1);
	}

	/* build address data structure */
	bzero((char *)&sockin, sizeof(sockin));
	sockin.sin_family = AF_INET;
	sockin.sin_addr.s_addr = INADDR_ANY;
	sockin.sin_port = htons(SERVER_PORT);
	
	/* setup passive open */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("server: socket");
		exit(1);
	}
	if ((bind(s, (struct sockaddr *)&sockin, sizeof(sockin))) < 0) {
		perror("server: bind");
		exit(1);
	}
	listen(s, MAX_PENDING);
	
	/* wait for connection, then receive and print text */
	while(1) {
		if ((new_s[i] = accept(s, (struct sockaddr *)&sockin, &len)) < 0) {
			perror("server: accept");
			exit(1);
		}else {
			fprintf(stderr, ANSI_COLOR_CYAN "Client %s connected from port %d\n\n" ANSI_COLOR_RESET, 
				inet_ntoa(sockin.sin_addr), ntohs(sockin.sin_port));
				
			exec = pthread_create(&th[i], NULL, THREAD, (void *)&new_s[i]);
			i++;		
		}
	}
	//pthread_exit(NULL);
	return(0);
}


