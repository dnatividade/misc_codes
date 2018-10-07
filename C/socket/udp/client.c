//Chap.1 (34)

////////////////
/* UDP Client */
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
		fprintf(stderr, "client: unknown host: %s\n", host);
		exit(1);
	}
	
	/* build address data structure */
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
	sin.sin_port = htons(SERVER_PORT);

	/* active open */
	if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror(ANSI_COLOR_RED "client: socket");
		exit(1);
	}

	printf(ANSI_COLOR_GREEN "Ready to send data to %s on port %d\n\n" ANSI_COLOR_RESET, host, SERVER_PORT);

	/* main loop: get and send lines of text */
	while (fgets(buf, sizeof(buf), stdin)) {
	
		buf[MAX_LINE-1] = '\0';
		len = strlen(buf) + 1;
		//send(s, buf, len, 0);
		sendto(s, buf, len, 0, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
	}

	return(0);	
}

