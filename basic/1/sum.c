#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>

#define STR_A_MAX_LEN (4096 + 1)
#define STR_B_MAX_LEN (2048 + 1)
#define STR_RES_MAX_LEN (4096 + 2)

char a[STR_A_MAX_LEN];
char b[STR_B_MAX_LEN];

bool readFile(const char* fpath, uint8_t* out, size_t maxOutLen)
{
	FILE *f = fopen(fpath, "r");
    if (!f)
		return false;

	if (fgets(out, maxOutLen, f) == NULL) {
		if (feof(f) == 0) {
			return false;
		}
	}
	fclose(f);

	return true;
}

int main(int argc, const char* argv[])
{
	if (argc < 4)
		return -1;

	if (!readFile(argv[1], a, STR_A_MAX_LEN))
		return -2;

	if (!readFile(argv[2], b, STR_B_MAX_LEN))
		return -3;

    int i;
    int carry;
    int dA;
    int dB;
    int tmpRes;
    char RES[STR_RES_MAX_LEN];
printf ("sum = %s\n", RES );
    for (i = 0, carry = 0 ; i < strlen(b); i++) {
        dA = a[strlen(a)-1 -i] -0x30;
        dB = b[strlen(b)-1 -i] -0x30;
        tmpRes = dA + dB + carry;
        RES[STR_RES_MAX_LEN -1 -i] = tmpRes % 10;
        carry = tmpRes / 10;

    } 
printf ("sum = %s\n", RES );

    for (i , carry  ; i < strlen(a); i++) {
        dA = a[strlen(a)-1 -i] -0x30;
        tmpRes = dA + carry;
        RES[STR_RES_MAX_LEN -1 -i] = tmpRes % 10;
        carry = tmpRes / 10;

    } 


        RES[STR_RES_MAX_LEN -1 -i] = carry;
printf ("sum = %s\n", RES );


    int fd = open(argv[3], O_RDWR | O_CREAT | O_APPEND);
	if (fd == -1)
		return -1;

	if (write(fd, RES, strlen(STR_RES_MAX_LEN)) < 0) {
		fprintf(stderr, "file write failed\n");
	}
	else {
		fprintf(stdout, "File written successfully\n");
	}
}
