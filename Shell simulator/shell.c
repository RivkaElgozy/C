#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#define FALSE 0
#define TRUE 1
#define HOST_NAME_MAX 64
#define MAX_CHARACTERS 510

char **func1 (char * str);
int enterString(char str[],int number);
void signal_handler_father(int sig);
static int counter;

int main()
{
    signal(SIGTSTP,signal_handler_father);//define the signal_handler_father to take care of 'SIGTSTP' signal
    signal(SIGCHLD,signal_handler_father);//define the signal_handler_father to take care of 'SIGCHLD' signal
    pid_t d,d2;
    int fp;
    char str[MAX_CHARACTERS+1];
    char hostname[HOST_NAME_MAX];
    int j,i,sum,sum2,sum3;
    const char done[]={'d','o','n','e','\0'};
    const char bg1[]={'b','g','\0'};
    const char temp[]={'|','\0'};
    const char out[]={'>','\0'};
    const char in[]={'<','\0'};
    int num,num2,contin,gototwosons,gobacktowhile,gotoin,gotoout;
    char **first;
    char **second;
    char **com1;
    const char s[2] = " ";
    int pipe_fd[2];
    while(TRUE)
    {
        num=0;
        num2=0;
        i=0;
        sum=0;
        sum2=0;
        sum3=0;
        gobacktowhile=FALSE;
        contin=TRUE;
        gototwosons=FALSE;
        gotoin=FALSE;
        gotoout=FALSE;
        gethostname(hostname, HOST_NAME_MAX);
        printf("%s@%s$ ",getlogin(), hostname);//print the user name and the host name
        contin=enterString(str,MAX_CHARACTERS);//get a string from the user and put it in 'str' ,that can contain a maximum of 510 characters, using the 'enterString' method
        if(contin==TRUE)//if the enterString method return FALSE which means that the user entered only spaces and \n, the program stops and goes back to ask the user to enter a string again
        {
            char** str2=func1(str);//create an array of pointers to string 'str2', that every string is a word that took from the 'str' string, using the 'func1' method
            if(strcmp(str2[0],done)==0)//if the first word was "done"
            {
                for(j=0;j<counter;j++)//release the 'str2' array:
                {
                    free(str2[j]);
                }
                free(str2);
                break;
            }
            if(strcmp(str2[0],bg1)==0)//if the first word was "bg"
            {
               kill(-1,SIGCONT);//send a SIGCONT to the detained process
               gobacktowhile=TRUE;//go back to while to get another command
            }
            if(gobacktowhile==FALSE)//if it does not need to go back to while to get another command
            {
                for(i=0;i<counter;i++)//go over the 'str2' string to check if there is a '|'
                {
                    ++sum;//keep the location of the '|' in the 'str2' string
                    if(strcmp(str2[i],temp)==0)//if there is a '|' in 'str2' string:
                    {
                        gototwosons=TRUE;//go to make the command by making 2 sons
                        break;
                    }
                }
                if(strcmp(str2[0],temp)==0||strcmp(str2[counter-1],temp)==0)//if the '|' is in the start or in the end of the command
                {
                    printf("command not found (can not put the '|' in the begin or the end of the command)\n");
                    gobacktowhile=TRUE;//go back to while to get another command
                    gototwosons=FALSE;//do not go to do the command by making 2 sons
                    gotoin=FALSE;//do not go to make a son that take information from a file
                    gotoout=FALSE;//do not go to make a son that put information in a file
                }
                if(gototwosons==FALSE&&gobacktowhile==FALSE)//if it does not need to go back to while to get another command and also does not need to do the command by making 2 sons
                {
                    for(i=0;i<counter;i++)//go over the 'str2' string to check if there is a '<'
                    {
                        ++sum2;//keep the location of the '<' in the 'str2' string
                        if(strcmp(str2[i],in)==0)//if there is a '<' in 'str2' string:
                        {
                            gotoin=TRUE;//go to make a son that take information from a file
                            break;
                        }
                    }
                    if(strcmp(str2[0],in)==0||strcmp(str2[counter-1],in)==0)//if the '<' is in the start or in the end of the command
                    {
                        printf("command not found (can not put the '<' in the begin or the end of the command)\n");
                        gobacktowhile=TRUE;//go back to while to get another command
                        gototwosons=FALSE;//do not go to do the command by making 2 sons
                        gotoin=FALSE;//do not go to make a son that take information from a file
                        gotoout=FALSE;//do not go to make a son that put information in a file
                    }
                    for(i=0;i<counter;i++)//go over the 'str2' string to check if there is a '>'
                    {
                        ++sum3;//keep the location of the '>' in the 'str2' string
                        if(strcmp(str2[i],out)==0)//if there is a '<' in 'str2' string:
                        {
                            gotoout=TRUE;//go to make a son that put information in a file
                            break;
                        }
                    }
                    if(strcmp(str2[0],out)==0||strcmp(str2[counter-1],out)==0)//if the '>' is in the start or in the end of the command
                    {
                        printf("command not found (can not put the '>' in the begin or the end of the command)\n");
                        gobacktowhile=TRUE;//go back to while to get another command
                        gototwosons=FALSE;//do not go to do the command by making 2 sons
                        gotoin=FALSE;//do not go to make a son that take information from a file
                        gotoout=FALSE;//do not go to make a son that put information in a file
                    }
                }
                if(gototwosons==FALSE&&gobacktowhile==FALSE&&gotoin==FALSE&&gotoout==FALSE)//if it does not need to do the command by making 2 sons and it does not need to go to while to get another command and it does not need to make a son that take information from a file and also does not need to make a son that put information in a file
                {
                    d=fork();//create a son (another process)
                    if(d==-1)//if there was a problem with creating the new process
                    {
                        printf("Error: could not make another process");
                        for(j=0;j<counter;j++)//release the 'str2' array:
                        {
                            free(str2[j]);
                        }
                        free(str2);
                        exit(1);
                    }
                    if(d==0)//if it's the son's process
                    {
                        num=execvp(str2[0],str2);//runs the command written as the first word in the received string and with the received arguments
                        if(num==-1)//if the first word was not a name of a command
                        {
                            printf("%s: command not found\n",str2[0]);
                            for(j=0;j<counter;j++)//release the 'str2' array:
                            {
                                free(str2[j]);
                            }
                            free(str2);
                            exit(0);
                        }
                    }
                    else//if it's the father's process
                    {
                        pause();//the father wait to his son to be finished
                        for(j=0;j<counter;j++)//release the 'str2' array:
                        {
                            free(str2[j]);
                        }
                        free(str2);
                    }
                }
                else if (gototwosons == TRUE&&gobacktowhile==FALSE)//if it needs to do the command by making 2 sons and it does not need to go back to while to get another command
                {
                    first=(char**)malloc((sum)*(sizeof(char*)));//sets the size of the string 'first' to be the number of words in 'str2' before the '|' plus one
                    if(first==NULL)//if 'malloc' could not allocate memory
                    {
                        printf("Error: could not allocate memory");
                        exit(1);
                    }
                    for(int i=0;i<sum-1;i++)//go over the 'first' array
                    {
                        first[i]=(char*)malloc((strlen(str2[i])+1)*sizeof(char));//sets every size of a pointer in 'first' to be the size of the string 'str2[i]' plus one
                        if(first[i]==NULL)//if 'malloc' could not allocate memory
                        {
                            printf("Error: could not allocate memory");
                            exit(1);
                        }
                        strcpy(first[i],str2[i]);//copy the 'str2' in index i to the first array in index i
                    }
                    first[sum-1]='\0';//put the '\0' in the last location in the first array
                    second=(char**)malloc((counter-sum+1)*(sizeof(char*)));//sets the size of the string 'second' to be the number of words in 'str2' after the '|' plus one
                    if(second==NULL)//if 'malloc' could not allocate memory
                    {
                        printf("Error: could not allocate memory");
                        exit(1);
                    }
                    for(int i=0;i<(counter-sum);i++)//go over the 'second' array
                    {
                        second[i]=(char*)malloc((strlen(str2[i+sum])+1)*sizeof(char));//sets every size of a pointer in 'first' to be the size of the string 'str2[i]' plus one
                        if(second[i]==NULL)//if 'malloc' could not allocate memory
                        {
                            printf("Error: could not allocate memory");
                            exit(1);
                        }
                        strcpy(second[i],str2[i+sum]);//copy the 'str2' in index i to the second array in index i
                    }
                    second[counter-sum]='\0';//put the '\0' in the last location in the second array
                    if(pipe(pipe_fd)==-1)//make a pipe, if it could not make a pipe:
                    {
                        printf("Error: could not make a pipe");
                        exit(1);
                    }
                    d=fork();//make son#1
                    if(d==-1)//if the fork did not work
                    {
                        printf("Error: could not make another process");
                        exit(1);
                    }
                    if(d==0)//if it is the son's process
                    {
                        close(pipe_fd[0]);//close the reading side of the pipe
                        dup2(pipe_fd[1],1);//make the stdout to be the writing side of the pipe
                        num=execvp(first[0],first);//runs the command written as the first word in the received string and with the received arguments
                        close(pipe_fd[1]);//close the writing side of the pipe
                        if(num==-1)//if the first word was not a name of a command
                        {
                            fprintf(stderr,"%s: command not found\n",first[0]);
                            for(j=0;j<counter;j++)//release the 'str2' array:
                            {
                                free(str2[j]);
                            }
                            free(str2);
                            for(j=0;j<sum-1;j++)//release the 'first' array:
                            {
                                free(first[j]);
                            }
                            free(first);
                            for(j=0;j<(counter-sum);j++)//release the 'second' array:
                            {
                                free(second[j]);
                            }
                            free(second);
                            exit(0);
                        }
                    }
                    else//if it is the father's process
                    {
                        d2=fork();//make son #2
                        if(d2==-1)//if the fork did not work
                        {
                            printf("Error: could not make another process");
                            exit(1);
                        }
                        if(d2==0)//if it is the son's process
                        {
                            close(pipe_fd[1]);//close the writing side of the pipe
                            dup2(pipe_fd[0],0);//make the stdin to be the reading side of the pipe
                            num2=execvp(second[0],second);//runs the command written as the first word in the received string and with the received arguments
                            close(pipe_fd[0]);//close the reading side of the pipe
                            if(num2==-1)//if the first word was not a name of a command
                            {
                                fprintf(stderr,"%s: command not found\n",second[0]);
                                for(j=0;j<counter;j++)//release the 'str2' array:
                                {
                                    free(str2[j]);
                                }
                                free(str2);
                                for(j=0;j<(counter-sum);j++)//release the 'second' array:
                                {
                                    free(second[j]);
                                }
                                free(second);
                                for(j=0;j<sum-1;j++)//release the 'first' array:
                                {
                                    free(first[j]);
                                }
                                free(first);
                                exit(0);
                            }
                        }
                        else//if it is the father's process
                        {
                            close(pipe_fd[0]);//close the reading side of the pipe
                            close(pipe_fd[1]);//close the writing side of the pipe
                            pause();//the father is waiting to his sons to be finished:
                            pause();
                            for(j=0;j<counter;j++)//release the 'str2' array:
                            {
                                free(str2[j]);
                            }
                            free(str2);
                            for(j=0;j<sum-1;j++)//release the 'first' array:
                            {
                                free(first[j]);
                            }
                            free(first);
                            for(j=0;j<(counter-sum);j++)//release the 'second' array:
                            {
                                free(second[j]);
                            }
                            free(second);
                        }
                    }
                }
                else if(gotoin==TRUE&&gotoout==FALSE&&gobacktowhile==FALSE)//if it needs to make a son that put information in a file and it does not need to go to while to get another command and it also does not need to make a son that take information from a file
                {
                    com1 =(char**)malloc((sum2)*(sizeof(char*)));//sets the size of the string 'com1' to be the number of words in 'str2' before the '|' plus one
                    if(com1==NULL)//if 'malloc' could not allocate memory
                    {
                        printf("Error: could not allocate memory");
                        exit(1);
                    }
                    for(int i=0;i<sum2-1;i++)//go over the 'com1' string
                    {
                        com1[i]=(char*)malloc((strlen(str2[i])+1)*sizeof(char));//sets every size of a pointer in 'com1' to be the size of the string 'str2[i]' plus one
                        if(com1[i]==NULL)//if 'malloc' could not allocate memory
                        {
                            printf("Error: could not allocate memory");
                            exit(1);
                        }
                        strcpy(com1[i],str2[i]);//copy the 'str2' in index i to the string 'com1' in index i
                    }
                    com1[sum2-1]='\0';//put the '\0' in the end of the 'com1' string
                    d=fork();//create a son (another process)
                    if(d==-1)//if the fork did not work
                    {
                        printf("Error: could not make another process");
                        exit(1);
                    }
                    if(d==0)//if it is the son's process
                    {
                        fp = open (str2[counter-1], O_RDONLY);//open the file that in the end of the 'str2' string for reading
                        if(fp==-1)//if there was a problem with opening the file
                        {
                            printf("Error: could not open the file\n");
                            for(int i=0;i<sum3-1;i++)//release the 'com1' string:
                            {
                                free(com1[i]);
                            }
                            free(com1);
                            for(int i=0;i<counter;i++)//release the 'str2' string:
                            {
                                free(str2[i]);
                            }
                            free(str2);
                            exit(1);
                        }
                        dup2(fp,0);//make the stdin to be the file (that open for reading only)
                        num=execvp(com1[0],com1);//runs the command written as the first word in the received string and with the received arguments
                        close(fp);//close the file
                        if(num==-1)//if the first word was not a name of a command
                        {
                            fprintf(stderr,"%s: command not found\n",com1[0]);
                            for(int i=0;i<sum2-1;i++)//release the 'com1' string:
                            {
                                free(com1[i]);
                            }
                            free(com1);
                            for(int i=0;i<counter;i++)//release the 'str2' string:
                            {
                                free(str2[i]);
                            }
                            free(str2);
                            exit(0);
                        }
                    }
                    else // father process
                    {
                        pause();//the father is waiting to the son's process to be finished
                        for(int i=0;i<sum2-1;i++)//release the 'com1' string:
                        {
                               free(com1[i]);
                        }
                        free(com1);
                        for(int i=0;i<counter;i++)//release the 'str2' string:
                        {
                            free(str2[i]);
                        }
                        free(str2);
                    }
                }
                else if(gotoout==TRUE&&gotoin==FALSE&&gobacktowhile==FALSE)//if it needs to make a son that take information from  a file and it does not need to go to while to get another command and it also does not need to make a son that put information in a file
                {
                    com1=(char**)malloc((sum3)*(sizeof(char*)));//sets the size of the string 'out' to be the number of words after the '|' plus one
                    if(com1==NULL)//if 'malloc' could not allocate memory
                    {
                        printf("Error: could not allocate memory");
                        exit(1);
                    }
                    for(int i=0;i<sum3-1;i++)//go over  the 'com1' string
                    {
                        com1[i]=(char*)malloc((strlen(str2[i])+1)*sizeof(char));//sets every size of a pointer in 'com1' to be the size of the string 'str2[i]' plus one
                        if(com1[i]==NULL)//if 'malloc' could not allocate memory
                        {
                            printf("Error: could not allocate memory");
                            exit(1);
                        }
                        strcpy(com1[i],str2[i]);//copy the 'str2' in index i to the string 'com1' in index i
                    }
                    com1[sum3-1]='\0';//put the '\0' in the end of the 'com1' string
                    d2=fork();//create a son (another process)
                    if(d2==-1)//if the fork did not work
                    {
                        printf("Error: could not make another process");
                        exit(1);
                    }
                    if(d2==0)//if it is the son's process
                    {
                        fp = open(str2[counter-1],O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);// open/create the file that in the end of the 'str2' string for writing
                        if(fp==-1)//if there was a problem with opening the file
                        {
                            printf("Error: could not open the file\n");
                            for(int i=0;i<sum3-1;i++)//release the 'com1' string:
                            {
                                free(com1[i]);
                            }
                            free(com1);
                            for(int i=0;i<counter;i++)//release the 'str2' string:
                            {
                                free(str2[i]);
                            }
                            free(str2);
                            exit(1);
                        }
                        dup2(fp,1);//make the stdout to be the file (that open for writing)
                        num2=execvp(com1[0],com1);//runs the command written as the first word in the received string and with the received arguments
                        close(fp);//close the file
                        if(num2==-1)//if the first word was not a name of a command
                        {
                            fprintf(stderr,"%s: command not found\n",com1[0]);
                            for(int i=0;i<sum3-1;i++)//release the 'com1' string:
                            {
                                free(com1[i]);
                            }
                            free(com1);
                            for(int i=0;i<counter;i++)//release the 'str2' string:
                            {
                                free(str2[i]);
                            }
                            free(str2);
                            exit(0);
                        }
                    }
                    else//if it is father process
                    {
                        pause();//the father is waiting to his son's process to be finished
                        for(int i=0;i<sum3-1;i++)//release the 'com1' string:
                        {
                            free(com1[i]);
                        }
                        free(com1);
                        for(int i=0;i<counter;i++)//release the 'str2' string:
                        {
                            free(str2[i]);
                        }
                        free(str2);
                    }
                }
                else if(gotoin==TRUE&&gotoout==TRUE&&gobacktowhile==FALSE)//if there was a'>' and a'<' in the received sentence
                {
                    printf("command not found (can not include a '<' and a '>' both)\n");
                }
            }
        }
    }
    return 0;
}
char** func1(char * str)
{
/*This method gets a string 'str' and create an array of pointers to string 'str2', that every string is a word that took from the 'str' string.in the end this method returns the 'str2' */
    int i=0;
    counter=0;
    const char s[2] = " ";
    while(i<MAX_CHARACTERS)
    {
        if(str[i]=='\0')
        {
            break;
        }
        if((str[i]!=' '&&str[i+1]=='\0')||(str[i]!=' '&&str[i+1]==' '))//if there is a space after a character or null after a character(it's another word)
        {
            ++counter;
        }
        ++i;
    }
    char** str2;
    str2=(char**)malloc((counter+1)*(sizeof(char*)));//sets the size of the string 'str2' to be the number of words plus one
    if(str2==NULL)//if 'malloc' could not allocate memory
    {
        printf("Error: could not allocate memory");
        exit(1);
    }
    char* token;
    i=0;
    token=strtok(str, s);//take the first word from 'str' and put it in 'token'
    while(token != NULL )//keep taking words from 'str' and put it in 'token' until its null
    {
        str2[i]=(char*)malloc((strlen(token)+1)*sizeof(char));//sets every size of a pointer in 'str2' to be the size of the string 'token' plus one
        if(str2[i]==NULL)//if 'malloc' could not allocate memory
        {
            printf("Error: could not allocate memory");
            exit(1);
        }
        strcpy(str2[i],token);//copy the 'token' string to the right pointer in 'str2'
        token = strtok(NULL, s);//take the next word from 'str' and put it in 'token'
        ++i;
    }
    str2[counter]='\0';//put null in the end of the 'str2' string
    return str2;
}
int enterString(char str[],int number)
{
/*This method gets an array of char('str') and a number ('number'), and gets characters from the user until he press the 'enter' and puts the string in the array 'str'.
This method will not insert more characters into the array than the received number ('number'), if the user enters more, the method will not refer to the continuation of the characters
and if there are spaces in the beginning it wont be in the 'str' too, this method also returns FALSE if the user enters only spaces and than \n, and returns TRUE otherwise*/
    char c;
    int i=0,flag=0;
    do
    {
        c=getchar();//get a character from the user
        if(c!=' ')//if the character is not space
        {
            ++flag;
        }
        if(c!='\n'&&i<number&&flag!=0)//if the character is not 'enter' and there is an empty place in the array and it is not space in the beginning
        {
            str[i]=c;//put the character in the right place in the array
            ++i;
        }
    }while((c!='\n'));//get characters from the user until he enters 'enter'
    if(i==0)
    {
        return FALSE;
    }
    str[i]='\0';//put the \0 in the end of the string
    return TRUE;
}
void signal_handler_father(int sig)
{
/*this function handler the SIGCHLD signal and the SIGTSTP signal, if there was a SIGTSTP signal the function does not do anything and if there was a SIGCHLD  signal
the function the function waits for any child process and returns immediately if no child has exited .*/
    if(sig==SIGCHLD)//if there was a SIGCHLD signal
    {
        waitpid(-1,NULL,WNOHANG);//waits for any child process and returns immediately if no child has exited
    }
}

