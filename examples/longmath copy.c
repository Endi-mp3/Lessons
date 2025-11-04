#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/errno.h>

int readFile(const char* fpath, uint8_t* out, uint16_t read_bytes)
{
	int fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -1;
	int r = 0;
	if (read(fd, out, read_bytes) < 0) {
		fprintf(stderr, "file read failed. Errno == %i\n", errno);
		r = -1;
	}
	close(fd);
	return r;
}

#define strA_LenMax (4097u)
#define strB_LenMax (2049u)
#define strRes_LenMax (strA_LenMax + 1)

int main(int argc, const char* argv[])
{

	uint8_t* strA = (uint8_t*)malloc(strA_LenMax);
	if (!strA)
		return -1;

	uint8_t* strB = (uint8_t*)malloc(strB_LenMax);
	if (!strB)
		goto error_free_A;

	
	if (argc < 4)
		goto error_free_A_B;

	if (readFile(argv[1], strA, strA_LenMax))
		goto error_free_A_B;

	if (readFile(argv[2], strB, strB_LenMax))
		goto error_free_A_B;

	

	uint16_t strA_Len = strnlen(strA, strA_LenMax);
	uint16_t strB_Len = strnlen(strB, strB_LenMax);

	uint8_t* strRes = (uint8_t*)malloc(strA_Len + 2); 

	if (!strRes)
		goto error_free_A_B;
	

	memset(strRes, 0, strRes_LenMax + 1);

	int i;

	strA_Len = (strA[strA_Len - 1] == '\n') ? strA_Len - 1 : strA_Len;
	strB_Len = (strB[strB_Len - 1] == '\n') ? strB_Len - 1 : strB_Len;

	if (strA_Len < strB_Len)
		goto error_free_A_B_RES;

	int carry = 0;
	for(i = 0; i < strB_Len; i++) {
		uint8_t dA = strA[strA_Len - 1 - i] - 0x30;
		uint8_t dB = strB[strB_Len - 1 - i] - 0x30;
		uint8_t tmpRes = dA + dB + carry;
		strRes[strRes_LenMax - 1 - i] = tmpRes % 10 + 0x30; // convert number to character back by adding '0'
		carry = tmpRes / 10;
	}

	for(; i < strA_Len; i++) {
		uint8_t dA = strA[strA_Len - 1 - i] - 0x30;
		uint8_t tmpRes = dA + carry;
		strRes[strRes_LenMax - 1 - i] = tmpRes % 10 + 0x30; // convert number to character back by adding '0'
		carry = tmpRes / 10;
	}

	if (carry)
		strRes[strRes_LenMax - 1 - i++] = '1';

	uint8_t* ptr = &strRes[strRes_LenMax - 1 - i];
	int fd = open(argv[3], O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (fd == -1) {
		fprintf(stderr, "file write failed\n");
		goto error_free_A_B_RES;
	}

	if (write(fd, ptr, strlen(ptr)) < 0) {
		fprintf(stderr, "file write failed\n");
		goto error_free_A_B_RES;
	}

	/*
	printf("  A:");
	for(int i = 0; i < strlen(ptr) - strA_Len; i++)
		printf("0");

	for(int i = 0 ; i < strA_Len; i++)
		printf("%c", strA[i]);

	printf("\n  B:");
	for(int i = 0; i < strlen(ptr) - strB_Len; i++)
		printf("0");

	for(int i = 0 ; i < strB_Len; i++)
		printf("%c", strB[i]);

	printf("\nRes:");
	for(int i = 0 ; i < strlen(ptr); i++)
		printf("%c", ptr[i]);

	printf("\n");

	*/
	return 0;

error_free_A_B_RES:
	free(strRes);
error_free_A_B:
	free(strB);
error_free_A:
	free(strA);
	return -1;
}
