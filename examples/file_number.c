#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/errno.h>



int main(int argc, char *argv[] ) 

{
if (argc != 3) {
 fprintf(stderr, "Usage: %s <filename> <line_number>\n", argv[0]);
 return -1;
}

int fd = open(argv[1], O_RDWR | O_CREAT | O_APPEND);
	if (fd == -1)
		return -1;

int Num_line = atoi(argv[2]);
if (Num_line < 0 ) {
     fprintf(stderr, "Usage: there is no such line\n");
     return 1;
}

char buffer[0];
int count = 0;










}    
