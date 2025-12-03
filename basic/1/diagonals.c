#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>

uint64_t Sum (int arg, int** array, int col)
{
	uint64_t sum = 0;
	switch (arg)
	{
	case 1:
		for (int i = 0; i < col; i++){
			for (int j = 0; j < col; j++){
				if (i > j) {
					sum += array[i][j];
				}
			}
		}
		break;
	case 2:
		for (int i = 0; i < col; i++){
			for (int j = 0; j < col; j++){
				if (i < j) {
					sum += array[i][j];
				}
			}
		}
		break;
	case 3:
		for (int i = 0; i < col; i++){
			for (int j = 0; j < col; j++){
				if (col - 1 - i < j) {
					sum += array[i][j];
				}
			}
		}
		break;
	case 4:
		for (int i = 0; i < col; i++){
			for (int j = 0; j < col; j++){
				if (col - 1 - i > j) {
					sum += array[i][j];
				}
			}
		}
		break;
	default:
		break;
	}
	return sum;
}


int main (int argc, const char* argv[]) 
{
	FILE *f = fopen(argv[2], "r");
	if (!f) {
		return -2;
	}
	int Column;
	fscanf(f, "%d", &Column );
	int Array[Column][Column];

	for (int i = 0; i < Column; i++) {
		for (int j = 0; j < Column; j++) {
			if (fscanf(f, "%d", &Array[i][j]) == 0) {
				perror("Unable to read value from file");
				return -1;
			}			
		}
	}

	printf("Result: %llu\n", Sum(argv[1][0] - '0', Array, Column));

	fclose(f);
	return 0;
	
}