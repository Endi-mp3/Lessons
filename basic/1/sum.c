#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define STR_A_MAX_LEN (4096 + 1)
#define STR_B_MAX_LEN (2048 + 1)

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



}
