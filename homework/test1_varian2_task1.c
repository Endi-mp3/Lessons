#include <stdio.h>
#include <stdint.h>

#define MAX 6
uint8_t arr[MAX] = {5, 4, 3, 2, 1, 0};


void Swap (uint8_t* arr, uint8_t i, uint8_t j ){
	uint8_t tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

void BubleSort (uint8_t* arr, uint8_t n){

	for (int i = 0; i - n ; i++){
		for(int j = 0; j - n + 1; j++){
			if (arr[j] > arr[j + 1]){
				Swap(arr, j,j+1);
			}
		}
	}
}


int main(void){
	BubleSort (arr, MAX);
	for (int i = 0; i < MAX; i++){
		printf ("%d ",arr[i]);
	}
}