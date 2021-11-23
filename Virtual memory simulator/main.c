#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#define PAGE_SIZE 4
#define NUM_OF_PAGES 20
#define MEMORY_SIZE 24
#define FALSE 0
#define TRUE 1
#include "Virtual_memory_simulator.h"
int main()
{
     page_descriptor page_table[NUM_OF_PAGES];
    char main_memory[MEMORY_SIZE];
    int logical, swap,input=1,add;
    char val,ch;
    init_system(page_table, main_memory, &logical, &swap);
    while(input==1||input==2)
    {
        printf("What do you whant to do? (press 1,2 or 3)\n1.load\n2.store\n3.exit\n");
        scanf("%d",&input);
        if(input==1)
        {
            printf("enter an address\n");
            scanf("%d",&add);
            val = load(add, page_table, main_memory, logical, swap);
        }
        else if (input==2)
        {
            printf("enter an address\n");
            scanf("%d",&add);
            fflush(stdin);
            printf("enter a character\n");
            //ch=getchar();
            scanf(" %c",&ch);
            store(add, ch, page_table, main_memory, logical, swap);
        }
    }
    close(swap);
    close(logical);
    print_memory(main_memory);


    return 0;
}
