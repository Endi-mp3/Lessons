#include <stdio.h>
#include <stdint.h>

void swap (uint8_t* array, uint8_t i, uint8_t idx) {

	int tmp = array[i];
	array[i] = array[idx];
	array[idx] = tmp;
	 
}

void sorting (uint8_t* array, uint32_t n)
{
	int i = 0;
	int j = 0;
    for (i = 0; i < n - 1; i++) {
		uint8_t idx = i;
		uint64_t MIN = UINT64_MAX;
		for (j = i + 1; j < n; j++ ) {
			if (array[i] < MIN) {
				idx = j;
				MIN = array[i];
			}
		swap (array, i, idx);
		}	 
	}
}