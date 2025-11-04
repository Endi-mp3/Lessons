#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct ListEntity_s;
typedef struct ListEntity_s ListEntity;
typedef struct List_s List;

// TODO: malloc can return NULL - add checks wherever needed
// ----
// TODO: 
// ptr = malloc(s);

// WRONG:
// ptr = realloc(s*2); <--- realloc also may return NULL.  In this case, we overwrite variable PTR and lose old allocation

// CORRECT:
// newPtr = realloc(s*2);
// if (newPtr)
//    ptr = newPtr;
// don't forget that ptr size is still the same if newPtr == NULL 

struct ListEntity_s
{   
    ListEntity* next;
    int age;
    char* name; // TODO
    //char name[64]; // OLD

    //int len;      // TODO 2
    //char name[0]; // TODO 2
};

struct List_s
{
    int count;
    ListEntity* list;
};

void list_addEntity(List* lst, const char* name, int age)
{
    ListEntity* ptrNewEntity = malloc(sizeof( ListEntity));
    memset(ptrNewEntity, 0, sizeof(ListEntity));
    if (lst->list == NULL) {
        lst->list = ptrNewEntity;
    }
    else {
        ListEntity* ptr = lst->list;
        while(ptr->next) 
        {
           ptr = ptr->next;
        }
        ptr->next = ptrNewEntity;    
    }
    lst->count++;
    ptrNewEntity->age = age;
    strcpy(ptrNewEntity->name, name);
    
}

void list_printEntity(ListEntity* ent)
{
        printf("name - %s\n age - %d\n", ent->name, ent->age);
 // one entity
}

void list_deleteEntity(List* lst, ListEntity* ent)
{
    if (!lst || !ent)
        return;
    
    ListEntity* ptr = lst->list;  
    if (ptr && ent == ptr) {
        lst->list = ptr->next;
        free(ptr);
        lst->count--;
        return;        
    }

    while (ptr && ptr->next != ent)
    {   
        ptr = ptr->next;
    }

    if (!ptr)
        return;

    ptr->next = ent->next;
    free(ent);
    lst->count--;
}

void list_editEntity(ListEntity* ent, const char*  new_name, int new_age)
{
    strcpy(ent->name, new_name);
    ent->age = new_age;
}

void list_init(List* lst)
{
    lst->count = 0;
    lst->list = NULL;
}

void list_cleanup(List* lst)
{   
    if (lst->count == 0)
        return;
    ListEntity* ptr = lst->list;
    ListEntity* ptrNext = ptr->next;
        
    while (!ptrNext)
    {
        ptrNext = ptr->next;
        free(ptr);
        ptr = ptrNext;
    }
    lst->count = 0;
    lst->list = NULL;
}

int main(int argc, const char* argv[])
{
    List lst;
    list_init(&lst);
    int count = 0;
    printf("Input number of pets\n");
    scanf("%d", &count);
    for(int i = 0; i < count; i++) {
        char tmpName[64] = { 0 };
        int tmpAge = 0;
        printf("\nInput pet's name: ");
        scanf("%s", tmpName);
        printf("\nInput pet's age: ");
        scanf("%d", &tmpAge);

        list_addEntity(&lst, tmpName, tmpAge);
    }

    ListEntity* current = lst.list;
    char cmd = 1;
    while(cmd != 0) {
        cmd = getchar();
        if (cmd == 'n') {
            current = current->next;
            if (current == NULL)
                current = lst.list;
        } else if (cmd == 'p') {
            list_printEntity(current);
        } else if (cmd == 's') {
            // print total amount of pets
            printf("Total amount: %i\n", lst.count);
        }  else if (cmd == 'e') {
            // ask for new name and age
            char tmpName[64] = { 0 };
            int tmpAge = 0;
            printf("\nInput pet's name: ");
            scanf("%s", tmpName);
            printf("\nInput pet's age: ");
            scanf("%d", &tmpAge);
            list_editEntity(current, tmpName, tmpAge);
        } else if (cmd == 'd') {
            // delete current pet and show next of empty
            list_deleteEntity(&lst, current);
            current = lst.list;
        }  else if (cmd == 'a') {
            // print info about all pets
            ListEntity* ptr = lst.list;
            while(ptr) {
                printf("Entity: %s %i\n", ptr->name, ptr->age);
                ptr = ptr->next;
            }
        } else if (cmd == 'c') {
            // add one more pet to the list and ask about data of it
            char tmpName[64] = { 0 };
            int tmpAge = 0;
            printf("\nInput pet's name: ");
            scanf("%s", tmpName);
            printf("\nInput pet's age: ");
            scanf("%d", &tmpAge);
            list_addEntity(&lst, tmpName, tmpAge);
        } else if (cmd == 27) {
            cmd = 0;
        } else {
            printf("Supported commands: n s e d a c p\n");
        }
    }
    list_cleanup(&lst);
    return 0;
}