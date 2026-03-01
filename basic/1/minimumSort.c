#include <stdint.h>
#include <stdio.h>

#define MAX (8)

uint8_t arrey[MAX] = {6, 1, 4, 2, 5, 7, 3, 8};


void swap(uint8_t* arr, uint8_t i, uint8_t j){
	uint8_t Sw = arr[i];
	arr[i] = arr[j];
	arr[j] = Sw;
}

void MinSort (uint8_t* arr, uint8_t n){
	for (uint8_t i = 0; i < MAX - 1; i++){
		uint8_t Min = i;
		for (uint8_t j = i + 1; j < MAX; j++){
			if(arr[j] < arr[Min])
			Min = j;
		}

		swap(arrey, i, Min);
	}
}

int main(void){
	MinSort(arrey, MAX);
	for (int i = 0; i < MAX; i++){
		printf ("%d ", arrey[i]);
	}
	printf("\n");
	return 0;
}