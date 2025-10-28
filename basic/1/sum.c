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
#define STR_RES_MAX_LEN (4096 + 3)


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

	char a[STR_A_MAX_LEN];
	char b[STR_B_MAX_LEN];
    char RES[STR_RES_MAX_LEN];

	if (!readFile(argv[1], a, STR_A_MAX_LEN))
		return -2;

	if (!readFile(argv[2], b, STR_B_MAX_LEN))
		return -3;

    uint16_t i;
    uint8_t carry;
    uint8_t dA;
    uint8_t dB;
    uint8_t tmpRes;
	memset(RES, 0, STR_RES_MAX_LEN);
	uint16_t lenA = strlen(a);
	uint16_t lenB = strlen(b);
	const uint16_t lenRes = STR_RES_MAX_LEN - 2;
	RES[lenRes] = '\n';

	if (a[lenA - 1] == '\n')
		lenA--;

	if (b[lenB - 1] == '\n')
		lenB--;

    for (i = 0, carry = 0 ; i < lenB; i++) {
        dA = a[lenA - 1 - i] - 0x30;
        dB = b[lenB - 1 - i] - 0x30;
        tmpRes = dA + dB + carry;
        RES[lenRes - 1 - i] = tmpRes % 10 + 0x30;
        carry = tmpRes / 10;
    }

    for (; i < lenA; i++) {
        dA = a[lenA - 1 - i] - 0x30;
        tmpRes = dA + carry;
        RES[lenRes - 1 - i] = tmpRes % 10 + '0';
        carry = tmpRes / 10;
    }

	RES[lenRes - 1 - i] = carry + 0x30;
	uint8_t *ptrRes = &RES[lenRes - 1 - i];
    int fd = open(argv[3], O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (fd == -1) {
		fprintf(stderr, "file open for result failed %s\n", argv[3]);
		return -1;
	}

	if (write(fd, ptrRes, strlen(ptrRes)) < 0) {
		fprintf(stderr, "file write failed\n");
		return -4;
	}
	return 0;
}
