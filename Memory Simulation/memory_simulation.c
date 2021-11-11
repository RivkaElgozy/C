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
#include "memory_simulation.h"
int check_address(int address);
int main_memory_location(page_descriptor page_table[], char main_memory[]);
void page_fault(int address, page_descriptor page_table[], char main_memory[] , int logicalmem_fd, int swapfile_fd);
void delete_page(page_descriptor page_table[], char main_memory[],int page_delete);
static int error_flag=FALSE;
static int first=0;
void init_system(page_descriptor page_table[], char main_memory[], int * logicalmem_fd, int * swapfile_fd)
{
/*This method gets a page_descriptor ('page_table') an array of characters (main_memory), pointer to the logical memory(logicalmem_fd) and a pointer to the replacement space(swapfile_fd).
This method open the "program.txt" file and creates a new file named "swap.txt" and put zeros in it, The array of characters is initialized to zeros and each page in the page_descriptor is initialized too(valid=0 dirty=0 frame=-1).*/
    int i,cl;
    ssize_t ret;
    const char c='0';
    *logicalmem_fd = open ("program.txt", O_RDONLY);//try to open the "program.txt" file for reading
    if(*logicalmem_fd==-1)//if there was a problem with opening the file or the file does not exist
    {
        perror("Error: could not open the file\n");
        exit(1);
    }
    *swapfile_fd = open ("swap.txt",O_CREAT|O_RDWR, S_IRUSR | S_IWUSR);//try to create the "swap.txt" file for writing and reading
    if(*swapfile_fd==-1)//if there was a problem with creating the file
    {
        perror("Error: could not create the file\n");
        cl=close(*logicalmem_fd);//close the "program.txt" file
        if(cl==-1)
        {
            perror("Error: could not close the file 'program.txt'\n");
        }
        exit(1);
    }
    for(i=0;i<MEMORY_SIZE;i++)//go over the array of characters and put '0' in each character in the array:
    {
        main_memory[i]='0';
    }
    for(i=0;i<NUM_OF_PAGES;i++)//each page in the page_descriptor is initialized: valid=0, dirty=0, frame=-1 :
    {
        page_table[i].valid=0;
        page_table[i].dirty=0;
        page_table[i].frame=-1;
    }
    for(i=0;i<(NUM_OF_PAGES*PAGE_SIZE);i++)//go over the "swap.txt" file and try to write 0 in it NUM_OF_PAGES*PAGE_SIZE times:
    {
        ret=write(*swapfile_fd,&c,1);//try to write 0 in the "swap.txt" file
        if(ret==-1)//if there was a writing problem
        {
            perror("Error: could not write to the file\n");
            cl=close(*swapfile_fd);//close the "swap.txt" file
            if(cl==-1)
            {
                perror("Error: could not close the file 'swap.txt'\n");
            }
            cl=close(*logicalmem_fd);//close the "program.txt" file
            if(cl==-1)
            {
                perror("Error: could not close the file 'program.txt'\n");
            }
            exit(1);
        }
    }
}
char load(int address, page_descriptor page_table[], char main_memory[] , int logicalmem_fd, int swapfile_fd)
{
/*This method gets an address a page_descriptor ('page_table') an array of characters (main_memory), the logical memory(logicalmem_fd) and the replacement space (swapfile_fd).
 This method returns the value from the main memory at the received address. If the page is not in the main memory the method will bring it to the main memory and if there is no place in it, it will make place according to 'FIFO' and then bring it .
 After bringing the page to the main memory the method returns the value from the main memory at the received address. If the address is incorrect the method will return with an error massage */
    int page,location,offset;
    if(check_address(address)==FALSE)//check if the address is correct using the 'check_address' method
    {
        printf("invalid address\n");
        return '\0';
    }
    page=(address/PAGE_SIZE);//page=address divided by page size
    if(page_table[page].valid==0)//if the page is not in the main memory
    {
        page_fault(address,page_table,main_memory, logicalmem_fd,swapfile_fd);//bring the page to the main memory using the 'page_fault' method
        if(error_flag==TRUE)//if the 'page_fault' method had a file error
        {
            error_flag=FALSE;
            return '\0';
        }
        return load(address,page_table,main_memory, logicalmem_fd,swapfile_fd);//call the 'load' method after bringing the page to the main memory (to return the value from the main memory at the received address)
    }
    else//if the page is in the main memory
    {
        offset=(address%PAGE_SIZE);//offset=address-(page)
        location=(page_table[page].frame*PAGE_SIZE)+offset;//location is the right place in the main memory that should be returned
        return main_memory[location];//return the right character from the main memory
    }
}
int check_address(int address)
{
/*This method gets an address and returns TRUE if the address is between 0 and NUM_OF_PAGES*PAGE_SIZE and FALSE otherwize*/
    if(address<0||address>=(NUM_OF_PAGES*PAGE_SIZE))//if the number is not between 0 and NUM_OF_PAGES*PAGE_SIZE
    {
        return FALSE;
    }
    return TRUE;
}
int main_memory_location(page_descriptor page_table[], char main_memory[])
{
/*This method gets a page_descriptor ('page_table') and an array of characters (main_memory).
This method returns the next empty frame in the main memory or -1 if there is not an empty frame in the main memory */
    int i,j,res;
    for(i=0;i<MEMORY_SIZE;i=i+PAGE_SIZE)//go over all the frames
    {
        res=i;
        for(j=0;j<NUM_OF_PAGES;j++)//go over each page in the page table
        {
            if(page_table[j].frame==(i/PAGE_SIZE))//if there is a page that has this current frame
            {
                res=-1;
                break;
            }
        }
        if(res!=-1)//if there is no page that has this current frame
        {
            return res;//return the frame
        }
    }
    return -1;
}
void page_fault(int address, page_descriptor page_table[], char main_memory[] , int logicalmem_fd, int swapfile_fd)
{
/*This method gets an address a page_descriptor ('page_table') an array of characters (main_memory), the logical memory(logicalmem_fd) and the replacement space (swapfile_fd).
This method brings the received address's page to the main memory if there is an empty frame, or make a place in the main memory according to 'FIFO' otherwise, and then bring it .*/
    int location,page,i,counter=0,page_delete,ls;
    char c;
    ssize_t ret,ret2;
    location=main_memory_location(page_table,main_memory);//put in the variable 'location' the next empty frame in the main memory or -1 if there is not an empty frame in the main memory, using the 'main_memory_location' method
    if(location==-1)//if there is not an empty frame in the main memory
    {
        for(i=0;i<NUM_OF_PAGES;i++)//go over each page in the page table
        {
            if(page_table[i].frame==first&&page_table[i].valid==1)//if the current page is the first page of all of the pages to be in the main memory
            {
                page_delete=i;//keep the location of this current page in the variable 'page_delete'
            }
        }
        if(page_table[page_delete].dirty==0)//if the page that are going to be deleted has not changed
        {
            delete_page(page_table,main_memory,page_delete);//make a place in the main memory to put the received address's page in the main memory according to 'FIFO', using the 'delete_page' method
        }
        else//if the page that are going to be deleted has changed
        {
            counter=0;
            for(i=first*PAGE_SIZE;i<(first*PAGE_SIZE)+PAGE_SIZE;i++)//go over the frame that needed to be copied to the "swap.txt" file
            {
                c=main_memory[i];
                ls=lseek(swapfile_fd,(page_delete*PAGE_SIZE)+counter,0);//go over the "swap.txt" file to the right place to write the frame
                if(ls==-1)
                {
                    perror("Error: could not write to the file\n");
                    error_flag=TRUE;
                    return;
                }
                ++counter;
                ret=write(swapfile_fd,&c,1);//try to write the frame in the "swap.txt" file
                if(ret==-1)//if there was a writing problem
                {
                    perror("Error: could not write to the file\n");
                    error_flag=TRUE;
                    return;
                }
            }
            delete_page(page_table,main_memory,page_delete);//make a place in the main memory to put the received address's page in the main memory according to 'FIFO', using the 'delete_page' method
        }
    }
    else//if there is an empty frame in the main memory
    {
        page=(address/PAGE_SIZE);//page=address divided by page size
        if(page_table[page].dirty==0)//if the page has not changed (the page should be copied from the logical memory)
        {
            counter=0;
            for(i=location;i<(location+PAGE_SIZE);i++)//go over the empty frame in the main memory
            {
                ls=lseek(logicalmem_fd,(page*PAGE_SIZE)+counter,0);//go over the "program.txt" file to the right place to read the frame
                if(ls==-1)
                {
                    perror("Error: could not read from the file\n");
                    error_flag=TRUE;
                    return;
                }
                ++counter;
                ret2=read(logicalmem_fd,&c,1);//try to read the frame from the "program.txt" file
                if(ret2==-1)//if there was a reading problem
                {
                    perror("Error: could not read from the file\n");
                    error_flag=TRUE;
                    return;
                }
                main_memory[i]=c;//put the frame in the right place in the main memory
            }
        }
        else//if the page has changed (the page should be copied from the replacement space)
        {
            counter=0;
            for(i=location;i<(location+PAGE_SIZE);i++)//go over the empty frame in the main memory
            {
                ls=lseek(swapfile_fd,(page*PAGE_SIZE)+counter,0);//go over the "swap.txt" file to the right place to read the frame
                if(ls==-1)
                {
                    perror("Error: could not read from the file\n");
                    error_flag=TRUE;
                    return;
                }
                ++counter;
                ret2=read(swapfile_fd,&c,1);//try to read the frame from the "swap.txt" file
                if(ret2==-1)//if there was a reading problem
                {
                    perror("Error: could not read from the file\n");
                    error_flag=TRUE;
                    return;
                }
                main_memory[i]=c;//put the frame in the right place in the main memory
            }
        }
        page_table[page].frame=location/PAGE_SIZE;//update the frame of the page
        page_table[page].valid=1;//Updates the valid of the page(=1)
    }
}
void delete_page(page_descriptor page_table[], char main_memory[],int page_delete)
{
/*This method gets a page_descriptor ('page_table') an array of characters (main_memory) and the page that should be deleted according to FIFO('page_delete').
This method deletes the right page from the main memory (puts zeros instead) and updates the valid and the frame of the page,it also updates the next frame that should be deleted according to FIFO*/
    int i;
    for(i=first*PAGE_SIZE;i<(first*PAGE_SIZE)+PAGE_SIZE;i++)//delete the page who is the first page of all of the pages to be in the main memory (puts zeros instead)
    {
        main_memory[i]='0';
    }
    page_table[page_delete].valid=0;//updates the valid of the page (=0)
    page_table[page_delete].frame=-1;//updates the frame of the page (=-1)
    ++first;//increases by 1 the variable that keeps the next frame to be deleted
    if(first*PAGE_SIZE==MEMORY_SIZE)//if the magnification at 1 caused the variable 'first' to be the size of the main memory
    {
        first=0;//the next frame to be deleted will be the 0
    }
}
void store(int address, char val, page_descriptor page_table[], char main_memory[] , int logicalmem_fd, int swapfile_fd)
{
/*This method gets an address ,a character ('val') ,a page_descriptor ('page_table') an array of characters (main_memory), the logical memory(logicalmem_fd) and the replacement space (swapfile_fd).
 This method changes the value at the received address to the received character in the main memory. If the page is not in the main memory the method will bring it to the main memory and if there is no place in it, it will make place according to 'FIFO' and then bring it .
 After bringing the page to the main memory the method changes the value at the received address to the received character in the main memory. If the address is incorrect the method will return with an error massage */
    int page,location,offset;
    if(check_address(address)==FALSE)//check if the address is correct using the 'check_address' method
    {
        printf("invalid address\n");
        return;
    }
    page=(address/PAGE_SIZE);//page=address divided by page size
    if(page_table[page].valid==0)//if the page is not in the main memory
    {
        page_fault(address,page_table,main_memory, logicalmem_fd,swapfile_fd);//bring the page to the main memory using the 'page_fault' method
        if(error_flag==TRUE)//if the 'page_fault' method had a file error
        {
            error_flag=FALSE;
            return;
        }
        store(address,val,page_table,main_memory, logicalmem_fd,swapfile_fd);//call the store method after bringing the page to the main memory (to change the value in the main memory at the received address to the received character)
    }
    else//if the page is in the main memory
    {
        offset=(address%PAGE_SIZE);//offset=address-(page)
        location=(page_table[page].frame*PAGE_SIZE)+offset;//location is the right place in the main memory that should be changed
        main_memory[location]=val;//put the received character in the right place in the main memory
        page_table[page].dirty=1;//update the dirty of the page that the page has changed (=1)
    }
}
void print_memory(char main_memory[])
{
/*This method gets an array of characters (main_memory) and print it so that each page is in a different line*/
    int i;
    for(i=0;i<MEMORY_SIZE;i++)//go over the main memory
    {
        if(((i+1)%PAGE_SIZE)==0)//if the current character is in the end of a page
        {
            printf("%c\n",main_memory[i]);
        }
        else//if the current character is in the end of a page
        {
            printf("%c ",main_memory[i]);
        }
    }
}

