#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/errno.h>

int main(int argc, char* argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Wrong parameters. Use %s <path_to_file>\n", argv[0]);
		return -1;
	}

	int fd = open(argv[1], O_RDWR | O_CREAT | O_APPEND);
	if (fd == -1)
		return -1;

	char testString[256];
	if (read(fd, testString, 255) < 0) {
		fprintf(stderr, "file read failed. Errno == %i\n", errno);
	}
	else {
		fprintf(stdout, "File read successfully. Content: %s\n", testString);
	}
	// some magic is here

	close(fd);
}
