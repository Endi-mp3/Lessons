#include <stdio.h>
#include <stdint.h>

#define MAX (9)

uint8_t arr[MAX] = {1, 6, 4, 2, 5, 7, 3, 8, 0}; // int

void swap (uint8_t* arr, uint8_t i, uint8_t j)
{
	uint8_t sw = arr[i];
	arr[i] = arr[j];
	arr[j] = sw;
}

void sort (uint8_t* arr,uint8_t n)
{
	for (uint8_t i = 0; i < n; i++){
		for(uint8_t j = 0; j < n - 1; j++ ){
			if (arr[j] < arr[j + 1]){
				swap (arr, j, j+1);
			}
		}
	}

}

int main (void)
{
	for (int i = 0; i < MAX; i++){
		printf ("%d ",arr[i]);
		//return 0;
	}
	printf("\n");
	sort(arr, MAX);

	for (int i = 0; i < MAX; i++){
		printf ("%d ", arr[i]);
		//return 0;
	}
	printf("\n");
	return 0;
}
