//Chap.1 (34)

////////////////
/* UDP Server */
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

#include "frufru.h"

#define MAX_LINE 256

int main(int argc, char *argv[]) {

	struct sockaddr_in sin;
	char buf[MAX_LINE];
	int len;
	int s;
	int SERVER_PORT;

	if (argc==2) {
		SERVER_PORT = atoi(argv[1]);
	}
	else {
		fprintf(stderr, "Usage: %s <port>\n\n", argv[0]);
		exit(1);
	}

	/* setup passive open */
	if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("server: socket");
		exit(1);
	}

	/* build address data structure */
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(SERVER_PORT);
	
	if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
		perror("server: bind");
		exit(1);
	}

	while (len = recv(s, buf, sizeof(buf), 0)) {
		fprintf(stderr, "<- %s", buf);
	}

	return(0);	
}

