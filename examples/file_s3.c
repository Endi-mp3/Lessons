#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#ifdef IN_DEBUG
#define DEBUG(...) printf("%s %i:", \
     __FUNCTION__, __LINE__); \
     printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif
typedef struct 
{
    int id;
    int x;
    int y;
    char **lines;
    int lineCount;
} Entity;

int main ()
{
    int id;
    int x;
    int y;
    int s;
    char str[256];
    FILE *f = fopen("myEntity.txt", "r");
    if (!f) {
        return 1;
    }
    int entityesCount = 0;
    Entity *entityes = NULL;
    entityes = malloc(16 * sizeof(Entity));
    
    while (fscanf (f, "%d %d %d %d\n", &id, &x, &y, &s) == 4) {
        int i = 0;
        Entity *e = &entityes[entityesCount++];
        e->id = id;
        e->x = x;
        e->y = y;
        e->lineCount = s;
        DEBUG("Create new entity: %i %i %i. Line count: %i\n", id, x, y, s);
        if (e->lineCount) {
            if (e->lineCount)
                e->lines = (char**)malloc(sizeof(char*) * e->lineCount);
            else
                e->lines = NULL;
            i = 0;
            while (i < e->lineCount) {
                if (!fgets(str, sizeof(str), f)) {
                    DEBUG("Here %i\n", e->id);         
                    break;
                }
                DEBUG("%i %i %s\n", i, e->id, str);

                if (strncmp(str, "//-", 3) == 0) {
                    DEBUG("Here \n");
                    DEBUG("Here %s %i\n", str, e->id);
                    break;
                }
                if (strncmp(str, "//", 2) == 0 ) {

                    DEBUG("Here %s %i\n", str, e->id);
                    continue;
                }
                size_t len = strlen(str);
                str[len - 1] = (str[len - 1] == '\n') ? ('\0') : (str[len - 1]);             
                e->lines[i++] = strndup(str, 256);
                DEBUG("i = %i %s\n", i - 1, e->lines[i - 1]);
            }
        }
        while (fgets(str,sizeof(str),f)) {
            if (strncmp(str, "//-", 3) == 0) {
                DEBUG("Here %i\n", e->id);
                break;
            }
            DEBUG("Skipped %i %s\n", entityesCount, str);
        }
    }
    fclose(f); 

    printf("Найденные entity в файле:\n");

    int i;
    for (i = 0; i < entityesCount; i++) {
        Entity *e = &entityes[i];  
        printf ("%i %d: (%d, %d)(%d):\n",i, e->id, e->x, e->y, e->lineCount);
        for (int j = 0; j < e->lineCount; j++) {
            printf ("%i: %s\n", j, e->lines[j]);
        }
        printf("-------------------------------------\n");
    }

    // entityes = malloc(16 * sizeof(Entity));;
    // for(i = 0; i < entityesCount; i++) {
    //     entityes[i].lines = malloc(0xDE);
    //     for(int j = 0; j < entityes[i].lineCount; j++) {
    //         entityes[i].lines[j] = malloc(256);
    //     }
    // }    

    for(i = 0; i < entityesCount; i++) {
        if (entityes[i].lineCount) {
            for(int j = 0; j < entityes[i].lineCount; j++) {
                if (entityes[i].lines[j]) 
                    free(entityes[i].lines[j]);
            }
            free(entityes[i].lines);
        }
    }
    free(entityes);
    
    return 0;
}