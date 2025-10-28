#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>


char a[4096];
char b[2048];
uint64_t Num1;
uint64_t Num2;
uint64_t Sum;


int main() 
{

FILE *f = fopen("a.txt", "r");
    if(!f) {
        return 1;
    }
         while (fgets(a,sizeof(a),f)) {
            if (strncmp(a, "\0", 2) == 0){
                break;
            }
         }
        printf ("Txt1 = %s\n", a );

        fclose(f);

FILE *h = fopen("b.txt", "r");
    if(!h) {
        return 1;
    }
         while (fgets(b,sizeof(b),h)) {
            if (strncmp(b, "\0", 2) == 0){
                break;
            }
         }
           printf ("Txt2 = %s\n", b );

        fclose(h);  





}