//https://codereview.stackexchange.com/questions/43914/client-server-implementation-in-c-sending-data-files

//#include <openssl/rsa.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "frufru.h"

#define BUF_SIZE 1492
char *HOST;
int  SERVER_PORT = 1414;
char PARAM[10];


//CLIENT RECEIVES FILE
// ./client <IPv4> <SERVER_PORT> <filename>
int main(int argc, char** argv) {
	struct hostent *hp;
	struct sockaddr_in sin;
	char *HOST;
	char from_filename[BUF_SIZE];
	char to_filename[BUF_SIZE];
	int s;
	int len;
	int SERVER_PORT;

	if (argc == 5) {
		strcpy(PARAM,argv[1]); //parameter (-cp, -rm...)
		HOST = argv[2]; //hostname or IPv4 address
		SERVER_PORT = atoi(argv[3]); //server port
		strcpy(from_filename,argv[4]); //filename to send
	}
	else if ((argc == 6)) {
		strcpy(PARAM,argv[1]); //parameter (-cp, -rm...)
		HOST = argv[2]; //hostname or IPv4 address
		SERVER_PORT = atoi(argv[3]); //server port
		strcpy(from_filename,argv[4]); //filename src to send
		strcpy(to_filename,argv[5]); //filename dst to send
	}
	else {
		printf("Usage: %s <parameter> <server_host> <server_port> <from_filename> [<to_filename> optional]\n", argv[0]);
		exit(EXIT_FAILURE);
	}	

	// Name resolution
	hp = gethostbyname(HOST);
	if (!hp) {
		printf("Impossible to resolve or find %s\n", HOST);
		exit(EXIT_FAILURE);
	}
	
	// Address data structure
	bzero((char*)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	bcopy(hp->h_addr, (char*)&sin.sin_addr, hp->h_length);
	sin.sin_port = htons(SERVER_PORT);

	// Create a socket
	int sockfd = 0;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf(ANSI_COLOR_RED "\n\nError creating socket\n\n" ANSI_COLOR_RESET);
		sleep(3);
		return EXIT_FAILURE;
	}

	// Try to connect
	if(connect(sockfd, (struct sockaddr*)&sin, sizeof(sin)) < 0){
		printf(ANSI_COLOR_RED "\n\nERROR: Connect Failed\n\n" ANSI_COLOR_RESET);
		sleep(3);
		return EXIT_FAILURE;
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// IF CREATE OR MODIFY FILE //
	if (strcmp(PARAM, "-cp") == 0) {
		send(sockfd, PARAM, sizeof(PARAM), 0);
		sleep(1);

		// Open file to send
		FILE *fp = fopen(from_filename, "rb");
		if (fp==NULL){
			printf("File open error");
			return EXIT_FAILURE;
		}
		//
		send(sockfd, from_filename, sizeof(from_filename), 0);

		// Read data from file and send it
		for (;;){
			// First read file in chunks of BUF_SIZE bytes
			unsigned char buff[BUF_SIZE] = {0};
			int nread = fread(buff, 1, BUF_SIZE, fp);
			printf("Bytes read %d \n", nread);

			// If read was success, send data
			if (nread > 0){
			    printf("Sending \n");
			    write(sockfd, buff, nread);
			}

			/*
			 * There is something tricky going on with read ..
			 * Either there was error, or we reached end of file.
			 */
			if (nread < BUF_SIZE){
			    if (feof(fp))
			        printf("End of file\n");
			    if (ferror(fp))
			        printf("Error reading\n");
			    break;
			}
		}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}// IF DELETE FILE //
	else if (strcmp(PARAM, "-rm") == 0) {
		send(sockfd, PARAM, sizeof(PARAM), 0);
		sleep(1);

		send(sockfd, from_filename, sizeof(from_filename), 0);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// IF RENAME FILE //
	else if (strcmp(PARAM, "-mv") == 0) {
		send(sockfd, PARAM, sizeof(PARAM), 0);
		sleep(1);

		send(sockfd, from_filename, sizeof(from_filename), 0);
		send(sockfd, to_filename, sizeof(to_filename), 0);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	close(sockfd);

	return EXIT_SUCCESS;
}





