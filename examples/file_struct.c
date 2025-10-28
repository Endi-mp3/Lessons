#include <stdio.h>
#include <string.h>
#include <stdint.h>


typedef struct 
{
int id;
int x;
int y;
int s;
char **lines;
int lineCount;
} Entity;



int main ()
{
    FILE *f = fopen("myEntity.txt", "r");
    if (!f) {
        return 1;
    }

int id;
int x;
int y;
int s;
char str[256];

printf("Найденные entity в файле:\n");

while (fscanf(f, "%d %d %d %d", &id, &x, &y, &s ) == 4) {
    printf ("%d: (%d,%d)(%d)", id, x, y, s);
   while (fgets(str, sizeof(str), f)) {
    if (strncmp(str, "//", 2) == 0){
        break;
    }
  
    printf ("  %s", str);
   }
   
    
}
fclose(f); 
return 0;
} 




 // fgets(str, sizeof(str), f);swq