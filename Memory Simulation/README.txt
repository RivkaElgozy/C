# Memory Simulation
==Description==
The program simulates the virtual memory management.  

==functions==
There are eight help methods:
Function1 (check_address) - This function gets an address and returns TRUE if the address is between 0 and NUM_OF_PAGES*PAGE_SIZE and FALSE otherwize
Function 2 (main_memory_location) - This fuction gets a page_descriptor ('page_table') and an array of characters (main_memory). This function returns the next empty frame in the main memory or -1 if there is not an empty frame in the main memory
Function 3(page_fault) - This function gets an address a page_descriptor ('page_table') an array of characters (main_memory), the logical memory(logicalmem_fd) and the replacement space (swapfile_fd). This function brings the received address's page to the main memory if there is an empty frame, or make a place in the main memory according to 'FIFO' otherwise, and then bring it  
Function 4 (delete_page) - This function gets a page_descriptor ('page_table') an array of characters (main_memory) and the page that should be deleted according to FIFO('page_delete'). This function deletes the right page from the main memory (puts zeros instead) and updates the valid and the frame of the page,it also updates the next frame that should be deleted according to FIFO
Function 5 (store)- This function gets an address ,a character ('val') ,a page_descriptor ('page_table') an array of characters (main_memory), the logical memory(logicalmem_fd) and the replacement space (swapfile_fd).
This function changes the value at the received address to the received character in the main memory. If the page is not in the main memory the method will bring it to the main memory and if there is no place in it, it will make place according to 'FIFO' and then bring it 
After bringing the page to the main memory the method changes the value at the received address to the received character in the main memory. If the address is incorrect the method will return with an error massage
Function 6 (load) - This function gets an address a page_descriptor ('page_table') an array of characters (main_memory), the logical memory(logicalmem_fd) and the replacement space (swapfile_fd).
 This function returns the value from the main memory at the received address. If the page is not in the main memory the method will bring it to the main memory and if there is no place in it, it will make place according to 'FIFO' and then bring it . After bringing the page to the main memory the method returns the value from the main memory at the received address. If the address is incorrect the method will return with an error massage 
 Function 7 (init_system)-This function gets a page_descriptor ('page_table') an array of characters (main_memory), pointer to the logical memory(logicalmem_fd) and a pointer to the replacement space(swapfile_fd) . This fuction open the "program.txt" file and creates a new file named "swap.txt" and put zeros in it, The array of characters is initialized to zeros and each page in the page_descriptor is initialized too(valid=0 dirty=0 frame=-1)
Function8 (print_memory) - This function gets an array of characters (main_memory) and print it so that each page is in a different line

==Program Files==
memory_simulation.c
memory_simulation.h
program.txt
README.txt (this file)

==How to compile?==
Compile: gcc main.c memory_simulation.c -o memory_simulation
Run: ./memory_simulation

==Input==
press 1 for loading, 2 for storing and 3 for exit. For loading enter an address (number) and for storing enter an address (number) and data (character)  

==Output==
The memory after pressing 'exit' (3).
