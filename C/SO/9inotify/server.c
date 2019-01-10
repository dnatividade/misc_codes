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
#include "frufru.h"

#define BUF_SIZE 1492
int  SERVER_PORT = 1414;


// SERVER RECEIVES FILE //
int main(int argc, char** argv) {

	if (argc == 2) {
		SERVER_PORT = atoi(argv[1]);
	}
	else {
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}	

	int listenfd = socket(AF_INET, SOCK_STREAM, 0);

	printf("Server started and listenig succesfully on port ");
	printf(ANSI_COLOR_RED "%s" ANSI_COLOR_RESET, argv[1]);
	printf(ANSI_COLOR_RESET "...\n");

	struct sockaddr_in serv_addr;
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERVER_PORT);

	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	if (listen(listenfd, 10) == -1){
		printf("Failed to listen\n");
		return -1;
	}

	for (;;) {
		int connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

		//Receive data in packets of BUF_SIZE bytes
		int bytesReceived = 0;
		char buff[BUF_SIZE];
		char buffTmp[BUF_SIZE];
		//////////////////////////////////////////////////
		bytesReceived = read(connfd, buff, BUF_SIZE);
		
		////////////////////////////////////////////////////////////////////////////////
		// IF CREATE OR MODIFY FILE //
		if (strcmp(buff, "-cp") == 0) {

			//read filename
			bytesReceived = read(connfd, buff, BUF_SIZE);
			printf("filename = [%s]\n", buff);
			/* Create file where data will be stored */
			FILE *fp = fopen(buff, "wb");
			if (NULL == fp){
				printf("Error to open file\n");
				return 1;
			}
			//////////////////////////////////////////////////
			memset(buff, '0', sizeof(buff));
			
			//read file content
			while ((bytesReceived = read(connfd, buff, BUF_SIZE)) > 0) {
				printf("[%dB]", bytesReceived);
				fwrite(buff, 1, bytesReceived, fp);
			}
			if (bytesReceived == 0){
				printf(ANSI_COLOR_GREEN "\nFile received\n\n" ANSI_COLOR_RESET);
			}
			
			//////////////////////////////////////////////////
			if (bytesReceived < 0){
				printf(ANSI_COLOR_RED "\nRead Error\n" ANSI_COLOR_RESET);
			}
			
			fclose(fp);
		}
		////////////////////////////////////////////////////////////////////////////////
		// IF DELETE FILE //
		else if (strcmp(buff, "-rm") == 0) {
			bytesReceived = read(connfd, buff, BUF_SIZE);
			unlink(buff);
		}
		////////////////////////////////////////////////////////////////////////////////
		// IF RENAME FILE //
		else if (strcmp(buff, "-mv") == 0) {
			//read source filename 
			bytesReceived = read(connfd, buff, BUF_SIZE);
			
			//read destination filename 
			bytesReceived = read(connfd, buffTmp, BUF_SIZE);
			
			rename(buff, buffTmp);
		}
		////////////////////////////////////////////////////////////////////////////////
	}

	return 0;
}





