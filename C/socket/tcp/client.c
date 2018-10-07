//Chap.1 (33)

////////////////
/* TCP Client */
////////////////

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "frufru.h"

#define MAX_LINE 256

int main(int argc, char *argv[]) {
	struct hostent *hp;
	struct sockaddr_in sin;
	char *host;
	char buf[MAX_LINE];
	char buf_recv[MAX_LINE];
	int s;
	int len;
	int SERVER_PORT;

	if (argc==3) {
		host = argv[1];
		SERVER_PORT = atoi(argv[2]);
	}
	else {
		fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
		exit(1);
	}

	/* translate host name into peer's IP address */
	hp = gethostbyname(host);
	if (!hp) {
		fprintf(stderr, "%s: unknown host: %s\n", argv[0], host);
		exit(1);
	}
	
	/* build address data structure */
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
	sin.sin_port = htons(SERVER_PORT);
	
	/* active open */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror(ANSI_COLOR_RED "client: socket");
		exit(1);
	}
	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0){
		perror(ANSI_COLOR_RED "client: connect");
		close(s);
		exit(1);
	}
	////////////////////////
	//here, i am connected//
	////////////////////////
	printf(ANSI_COLOR_GREEN "Connected to %s on port %d\n\n" ANSI_COLOR_RESET, host, SERVER_PORT);

	/* main loop: get and send lines of text */
	while (fgets(buf, sizeof(buf), stdin)) {
		buf[MAX_LINE-1] = '\0';
		len = strlen(buf) + 1;
		send(s, buf, len, 0);
	}
	
	return(0);
}

