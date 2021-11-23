# Shell simulator

==Description==
The program simulates the shell of Linux with advanced commands like ^z, bg, commands with | , and commands with < or > 


==functions==
There are three help methods:
Function1 (func1) - this function gets a sentence and create an array of pointers to string , that every string is a word that took from the recieved sentence. in the end this function returns the new string
Function 2 (enterString) - this function gets a sentence from the user
Function 3(signal_handler_father) - this function handler the SIGCHLD signal and the SIGTSTP signal, if there was a SIGTSTP signal the function does not do anything and if there was a SIGCHLD  signal
the function the function waits for any child process and returns immediately if no child has exited 
  
==Program Files==
shell.c

==How to compile?==
Compile: gcc shell.c -o shell
Run: ./shell

==Input==
You can enter any sentence you want as follows:
There must be a space between the words even if the "word" is | or > etc., The sentence can contain a maximum of 510 characters  ,do not insert \t,'\r etc. between the words (meaning there will be a clean space between the words)
at the end of typing the sentence press Enter to receive the output
sentence for example: echo Hello World | wc -w
p.s. to stop the program enter the word: done

==Output==
If the input was :  echo Hello World | wc -w ,the output will be : 2
