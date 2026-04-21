#include <stdint.h>
#include <stdio.h>


void insertionSort(uint8_t* arr,uint8_t n){
	for (int i = 0; i < n; i++){
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
	printf ("Введите размер массива\n");
	int N;
	scanf ("%d", &N);
	printf ("Заполните массив данными\n");
	uint8_t arr[N];
	for (int i = 0; i < N; i++ ){
	scanf ("%hhu",&arr[i]);
	}
	insertionSort(arr,N);
	printf ("Отсортированный массив\n");
	for (int i = 0; i < N; i++){
		printf ("%hhu ", arr[i]);
	}
}