#include <stdint.h>
#include <stdio.h>

#define MAX (8)

uint8_t arrey[MAX] = {6, 1, 4, 2, 5, 7, 3, 8};


void insrt_Sort (uint8_t* arr, uint8_t n){
	for(int i = 1; i < n ; ++i){

		int k = i;
		while (k > 0 && arr[k - 1] > arr[k]){
			int tmp = arr[k - 1];
			arr[k - 1] = arr[k];
			arr[k] = tmp;
			k -= 1;
		}
	}
	
}


int main(void){
	insrt_Sort(arrey, MAX);
	for (int i = 0; i < MAX; i++){
		printf ("%d ", arrey[i]);
	}
	printf("\n");
	return 0;
}
