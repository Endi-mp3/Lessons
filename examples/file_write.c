#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>

int main(int argc, char* argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Wrong parameters. Use %s <path_to_file>\n", argv[0]);
		return -1;
	}

	int fd = open(argv[1], O_RDWR | O_CREAT | O_APPEND);
	if (fd == -1)
		return -1;

	const char testString[256] = "22222222222222222222\n";
	if (write(fd, testString, strlen(testString)) < 0) {
		fprintf(stderr, "file write failed\n");
	}
	else {
		fprintf(stdout, "File written successfully\n");
	}
	// some magic is here

	close(fd);
}
