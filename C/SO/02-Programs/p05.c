//A2-05
//Escrever um programa que mostre o conteudo de um arquivo TXT. Usando MMAP.

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void banner(void) {
	printf("------\n");
	printf("A2-P05\n");
	printf("------\n\n");
	
	printf("-------------------------\n");
	printf("Open TXT file with MMAP()\n");
	printf("-------------------------\n\n");
}


int main(int argc, char *argv[]) {
	char *addr;
	int fd;
	struct stat sb;
	off_t offset, pa_offset;
	size_t length;
	ssize_t s;

	banner();

	if (argc != 2) {
		printf("\nUsage: %s <file_name>\n", argv[0]);
		printf("E.g.: %s file.txt\n\n", argv[0]);	
		return EXIT_FAILURE;
	}
	

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	if (fstat(fd, &sb) == -1) {          /* To obtain file size */
		perror("fstat");
		exit(EXIT_FAILURE);
	}
		

	offset = 0;
	printf("sysconf(): %d\n", sysconf(_SC_PAGE_SIZE));
	pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);
	printf("pa_offset: %d\n", pa_offset);
	/* offset for mmap() must be page aligned */

	if (offset >= sb.st_size) {
		fprintf(stderr, "offset is past end of file\n");
		exit(EXIT_FAILURE);
	}

	length = sb.st_size - offset;

	addr = mmap(NULL, length + offset - pa_offset, PROT_READ, MAP_PRIVATE, fd, pa_offset);

	if (addr == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}

	s = write(STDOUT_FILENO, addr + offset - pa_offset, length);
	if (s != length) {
		if (s == -1) {
			perror("write");
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "partial write");
		exit(EXIT_FAILURE);
	}

	munmap(addr, length + offset - pa_offset);
	close(fd);

	exit(EXIT_SUCCESS);
}


