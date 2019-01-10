////////////////////////
// _9_ Inotify /////////
// Nine Is Not Efficient
/////////////////////////////////////////////
//struct inotify_event {
//	int      wd;       //Watch descriptor
//	uint32_t mask;     //Mask describing event
//	uint32_t cookie;   //Unique cookie associating related events (for rename(2))
//	uint32_t len;      // Size of name field
//	char     name[];   // Optional null-terminated name
//}
/////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
//#include <pthread.h>
//#include <semaphore.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include "frufru.h"

typedef struct inotify_event inotify_event_str;
#define BUF_LEN (10 * (sizeof(inotify_event_str) + NAME_MAX + 1))

//semafores
//semaphore mutex;
//semaphore s[1000];

char *HOST;
int  SERVER_PORT = 1414;

char MV_FROM[256], MV_TO[256];

void banner(void) {
	printf("------\n");
	printf("A6-P01\n");
	printf("------\n\n");
	
	printf("---------------------\n");
	printf("_9_ INOTIFY\n");
	printf("Nine Is Not Efficient\n");
	printf("---------------------\n\n");
}

void handler(inotify_event_str *i){
	if (i->cookie > 0)
		printf("cookie =%d; ", i->cookie);

//	if (i->mask & IN_ACCESS)        printf(ANSI_COLOR_CYAN   "[IN_ACCESS] " ANSI_COLOR_RESET);
//	if (i->mask & IN_ATTRIB)        printf(ANSI_COLOR_RESET  "[IN_ATTRIB] " ANSI_COLOR_RESET);
//	if (i->mask & IN_CLOSE_NOWRITE) printf(ANSI_COLOR_BLUE   "[IN_CLOSE_NOWRITE] " ANSI_COLOR_RESET);
	if (i->mask & IN_CLOSE_WRITE)   printf(ANSI_COLOR_YELLOW "[IN_CLOSE_WRITE] " ANSI_COLOR_RESET);
	if (i->mask & IN_CREATE)        printf(ANSI_COLOR_GREEN  "[IN_CREATE] " ANSI_COLOR_RESET);
	if (i->mask & IN_DELETE)        printf(ANSI_COLOR_RED    "[IN_DELETE] " ANSI_COLOR_RESET);
	if (i->mask & IN_DELETE_SELF)   printf(ANSI_COLOR_RED    "[IN_DELETE_SELF] " ANSI_COLOR_RESET);
//	if (i->mask & IN_IGNORED)       printf(ANSI_COLOR_RESET  "[IN_IGNORED] " ANSI_COLOR_RESET);
//	if (i->mask & IN_ISDIR)         printf(ANSI_COLOR_WHITE  "[IN_ISDIR] " ANSI_COLOR_RESET);
	if (i->mask & IN_MODIFY)        printf(ANSI_COLOR_MAGENTA"[IN_MODIFY] " ANSI_COLOR_RESET);
	if (i->mask & IN_MOVE_SELF)     printf(ANSI_COLOR_RED    "[IN_MOVE_SELF] " ANSI_COLOR_RESET);
	if (i->mask & IN_MOVED_FROM)    printf(ANSI_COLOR_MAGENTA"[IN_MOVED_FROM] " ANSI_COLOR_RESET);
	if (i->mask & IN_MOVED_TO)      printf(ANSI_COLOR_MAGENTA"[IN_MOVED_TO] " ANSI_COLOR_RESET);
//	if (i->mask & IN_OPEN)          printf(ANSI_COLOR_CYAN   "[IN_OPEN] " ANSI_COLOR_RESET);
//	if (i->mask & IN_Q_OVERFLOW)    printf(ANSI_COLOR_RED    "[IN_Q_OVERFLOW] " ANSI_COLOR_RESET);
//	if (i->mask & IN_UNMOUNT)       printf(ANSI_COLOR_RED    "[IN_UNMOUNT] " ANSI_COLOR_RESET);

	if (i->len > 0)
		printf("%s ", i->name);
	
	///////////////////////////////////////////////////////////
	//CREATED OR MODIFIED FILES
	if ( (i->mask & IN_CLOSE_WRITE) || (i->mask & IN_CREATE) ) {
		char *createCmdTmp = "./client -cp %s %d %s"; // ./client <server_host> <server_port> <filename>
		char createCmd[256];
		sprintf(createCmd, createCmdTmp, HOST, SERVER_PORT, i->name);	
		system(createCmd);
	}
	///////////////////////////////////////////////////////////
	//DELETE FILES
	else if (i->mask & IN_DELETE) {
		char *createCmdTmp = "./client -rm %s %d %s"; // ./client <server_host> <server_port> <filename>
		char createCmd[256];
		sprintf(createCmd, createCmdTmp, HOST, SERVER_PORT, i->name);	
		system(createCmd);
	}
	///////////////////////////////////////////////////////////
	//RENAME FILES
	else if (i->mask & IN_MOVED_FROM) {
		strcpy(MV_FROM, i->name);
	}
	else if (i->mask & IN_MOVED_TO) {
		strcpy(MV_TO, i->name);
		// ./client <server_host> <server_port> <from_filename> <to_filename>
		char *createCmdTmp = "./client -mv %s %d %s %s"; 
		char createCmd[512];
		sprintf(createCmd, createCmdTmp, HOST, SERVER_PORT, MV_FROM, MV_TO);
		system(createCmd);
		
		strcpy(MV_FROM, "");
		strcpy(MV_TO, "");
	}
	///////////////////////////////////////////////////////////
}


int main(int argc, char *argv[]) {
	int fd, wd;
	char buf[BUF_LEN];
	ssize_t returnRead;
	char *buf_ptr;
	inotify_event_str *event;
	
	banner();

	if (argc != 4) {
		printf("Usage: %s <directory/> <server_host> <server_port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	//define connection parameters
	HOST = argv[2]; //hostname or IPv4 address
	SERVER_PORT = atoi(argv[3]); //server port

	//create inotify
	fd = inotify_init();
	if (fd == -1){
		printf("ERROR: inotify_init\n");
		exit(EXIT_FAILURE);
	}

	wd = inotify_add_watch(fd, argv[1], IN_ALL_EVENTS);
	if (wd == -1){
	    printf("ERROR: Directory not found (inotify_add_watch error)\n");
		exit(EXIT_FAILURE);
	}
	printf(ANSI_BGCOLOR_YELLOW_L ANSI_COLOR_BLACK "Monitoring [ %s ]" ANSI_COLOR_RESET, argv[1]);
	printf(ANSI_COLOR_RESET "\n" ANSI_COLOR_RESET);


	//Loop
	while (1) {
		returnRead = read(fd, buf, BUF_LEN);
		
		if (returnRead == 0)
		    printf("End of file\n");

		if (returnRead == -1){
		    printf("ERROR: read\n");
			exit(EXIT_FAILURE);
		}
		
		printf("%d bytes read -> ", returnRead);

		for (buf_ptr = buf; buf_ptr < buf + returnRead; ){
		    event = (inotify_event_str *)buf_ptr;
		    handler(event);

		    buf_ptr += sizeof(inotify_event_str) + event->len;
		}
		printf("\n");
	}
	
	exit(EXIT_SUCCESS);
}


