#include <stdio.h> 
#include <stdlib.h>


int main(void){
	int N;
	printf ("Введите размер массива");
	scanf ("%d", &N);


	int *mas1 = malloc (N * sizeof(int));
	if (mas1 == 0){
		printf ("Память не выделенна");
		return 1;
	}

	printf ("Заполните массив данными");
	for (int i = 0;i < N; i++){
		scanf ("%d", &mas1[i]);
	}

	int *mas2 = malloc (N * sizeof(int));
	if (mas1 == 0){
		printf ("Память не выделенна");
		return 1;
	}

	for (int i = 0; i < N; i++){
		mas2[i] = (mas1[i] * 2); 
	}
	printf ("Второй масив получился\n");
	for (int i = 0; i < N; i++){
		printf ("%d", mas2[i]);
	}
}