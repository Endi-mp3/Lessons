#include <stdint.h>
#include <string.h>
#include <stdio.h>
#define MAX_ENT 32

struct fifo
{
	uint32_t buff[MAX_ENT];
	uint8_t idxReader;
	uint8_t idxWriter;
}; 

struct fifo ls_first = {{0}, 0, 0,};


int add (struct fifo *f, uint32_t entity){
	int next = (f->idxWriter + 1) % MAX_ENT;
	if ((next) == (f->idxReader)) {
		return -1;		
	}

	f->buff[f->idxWriter] = entity;
	f->idxWriter = next;
	
	return 0;

}

int get (struct fifo *f, uint32_t *entity){

	if ((f->idxReader) == (f->idxWriter)){
		return -1;
	}
	*entity = f->buff[f->idxReader];
	f->idxReader = (f->idxReader + 1)% MAX_ENT;
	return 0;
}


int print(struct fifo *f ){
	if (f->idxReader != f->idxWriter){
		int i = f->idxReader;
		while (i != f->idxWriter) {
			printf ("%d ", f->buff[i]);
			i = (i + 1) % MAX_ENT;
		}
		printf ("\n");
	}
	return 0;
}



int main(void){
    char cmd[16];
    uint32_t Number;
    printf ("comand\nadd-new num\nget-unistal\nprint-printing\nexit-wxit program\n");

    while (scanf("%15s", cmd) == 1) {
		

        if (strcmp(cmd, "add") == 0) {
            if (scanf("%u", &Number) != 1) {
                printf("error\n");
                return 0;
            }
            if (add(&ls_first, Number) < 0) {
                printf("error\n");
            }
        }

        else if (strcmp(cmd, "get") == 0) {
            if (get(&ls_first, &Number) < 0) {
                printf("error\n");
            } else {
                // Если нужно выводить извлечённый элемент:
                // printf("%u\n", Number);
            }
        }

        else if (strcmp(cmd, "print") == 0) {
            print(&ls_first);
        }

        else if (strcmp(cmd, "exit") == 0) {
            break;
        }
    }

    return 0;
}
