#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
// Глобальные переменные
const char* global_answer = "42";       // инициализированная глобальная константа
int global_uninit;          // неинициализированная (BSS)
const char* global_answer2 = "Second time - 42";       // инициализированная глобальная константа
int fun(int b)
{
	int a = 0 + b;
	return a;
}

int main(void)
{
    int local_var = 10;     // локальная переменная (стек)
    uint8_t *heap_var;          // указатель на кучу

    heap_var = (uint8_t *)malloc(100); // выделяем память в куче
    if (heap_var == NULL) {
        printf("Ошибка выделения памяти\n");
        return 1;
    }
	memset(heap_var, 0, 100);
	strcpy(heap_var, "123123", 6);

    printf("Адрес кода (функция main):   %p\n", (void*)main);
    printf("Адрес global_answer:       %p\n", (void*)&global_answer);
    printf("Адрес global_answer2:       %p\n", (void*)&global_answer2);
    printf("Адрес BSS:     %p\n", (void*)&global_uninit);
    printf("Адрес локальной переменной:  %p\n", (void*)&local_var);
    printf("Адрес переменной в куче:     %p\n", (void*)heap_var);

    free(heap_var); // освобождаем память
    return 0;
}

