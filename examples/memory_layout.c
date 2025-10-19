#include <stdio.h>
#include <stdlib.h>

// Глобальные переменные
const char* global_answer = "42";       // инициализированная глобальная константа
int global_uninit;          // неинициализированная (BSS)
const char* global_answer2 = "Second time - 42";       // инициализированная глобальная константа

int main(void) {
    int local_var = 10;     // локальная переменная (стек)
    int *heap_var;          // указатель на кучу

    heap_var = (int *)malloc(sizeof(int)); // выделяем память в куче
    if (heap_var == NULL) {
        printf("Ошибка выделения памяти\n");
        return 1;
    }
    *heap_var = 99;

    printf("Адрес кода (функция main):   %p\n", (void*)main);
    printf("Адрес global_answer:       %p\n", (void*)&global_answer);
    printf("Адрес global_answer2:       %p\n", (void*)&global_answer2);
    printf("Адрес BSS:     %p\n", (void*)&global_uninit);
    printf("Адрес локальной переменной:  %p\n", (void*)&local_var);
    printf("Адрес переменной в куче:     %p\n", (void*)heap_var);

    free(heap_var); // освобождаем память
    return 0;
}

