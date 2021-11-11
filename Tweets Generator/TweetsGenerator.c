#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORDS_IN_SENTENCE_GENERATION 20
#define MAX_WORD_LENGTH 100
#define MAX_SENTENCE_LENGTH 1000
#define TRUE 0
#define FALSE 1

typedef struct WordStruct {
    char *word;
    struct WordProbability *prob_list;
    int WordProbabilityLength;
    int textOccurrences;
    //... Add your own fields here
} WordStruct;

typedef struct WordProbability {
    struct WordStruct *word_struct_ptr;
    int occurrence;
    //... Add your own fields here
} WordProbability;

/************ LINKED LIST ************/
typedef struct Node {
    WordStruct *data;
    struct Node *next;
} Node;

typedef struct LinkList {
    Node *first;
    Node *last;
    int size;
} LinkList;

/**
 * Add data to new node at the end of the given link list.
 * @param link_list Link list to add data to
 * @param data pointer to dynamically allocated data
 * @return 0 on success, 1 otherwise
 */
int add(LinkList *link_list, WordStruct *data)
{
    Node *new_node = (Node*) malloc(sizeof(Node));
    if (new_node == NULL)
    {
        return 1;
    }
    *new_node = (Node){data, NULL};
    if (link_list->first == NULL)
    {
        link_list->first = new_node;
        link_list->last = new_node;
    }
    else
    {
        link_list->last->next = new_node;
        link_list->last = new_node;
    }
    link_list->size++;
    return 0;
}
/*************************************/

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number)
{
    return rand()%max_number;
}

/**
 * Choose randomly the next word from the given dictionary, drawn uniformly.
 * The function won't return a word that end's in full stop '.' (Nekuda).
 * @param dictionary Dictionary to choose a word from
 * @return WordStruct of the chosen word
 */
WordStruct *get_first_random_word(LinkList *dictionary)
{
    int random_number,i;
    Node *ptr;
    while (1)
    {
        ptr = dictionary->first;
        random_number = get_random_number(dictionary->size);//randomly choose a number between 0 to dictionary->size
        for (i = 0; i < random_number; i++)
        {
            ptr = ptr->next;
        }
        if((ptr->data->word[strlen(ptr->data->word)-1])!='.')//if the random word has no dot at the end
        {
            return ptr->data;
        }
    }
}

/**
 * Choose randomly the next word. Depend on it's occurrence frequency
 * in word_struct_ptr->WordProbability.
 * @param word_struct_ptr WordStruct to choose from
 * @return WordStruct of the chosen word
 */
WordStruct *get_next_random_word(WordStruct *word_struct_ptr)
{
    int i,random_number;
    random_number = get_random_number(word_struct_ptr->textOccurrences);//randomly choose a number between 0 to word_struct_ptr->textOccurrences
    for (i = 0; i < (word_struct_ptr->WordProbabilityLength); i++)//go over the probabilityList of the received WordStruct
    {
        if(((word_struct_ptr->prob_list[i].occurrence))>=random_number)//if the occurrence is equal or bigger than random_number
        {
            return (word_struct_ptr->prob_list[i].word_struct_ptr);
        }
        else//if the occurrence is smaller than random_number
        {
            random_number=random_number-(word_struct_ptr->prob_list[i].occurrence);//decrease the random_number
        }
    }
    return NULL;
}

/**
 * Receive dictionary, generate and print to stdout random sentence out of it.
 * The sentence most have at least 2 words in it.
 * @param dictionary Dictionary to use
 * @return Amount of words in printed sentence
 */
int generate_sentence(LinkList *dictionary)
{
    WordStruct *first_word,*next_word;
    int j;
    first_word=get_first_random_word(dictionary);//get the first word of the sentence
    next_word=first_word;
    printf("%s ",first_word->word);
    for(j=1;j<MAX_WORDS_IN_SENTENCE_GENERATION;j++)//create the continuation of the sentence
    {
        if(next_word==NULL||next_word->WordProbabilityLength==0)//if the word should be the last word in the sentence(does not have a probabilityLIst)
        {
            return j;
        }
        next_word=get_next_random_word(next_word);//calculate the next word
        printf("%s ",next_word->word);
    }
    return j;
}

/**
 * Gets 2 WordStructs. If second_word in first_word's prob_list,
 * update the existing probability value.
 * Otherwise, add the second word to the prob_list of the first word.
 * @param first_word
 * @param second_word
 * @return 0 if already in list, 1 otherwise.
 */
int add_word_to_probability_list(WordStruct *first_word,WordStruct *second_word)
{
    int i;
    for(i=0;i<(first_word->WordProbabilityLength);i++)//go over the probabilityList of the first time to check if the second word is there
    {
        if(strcmp(first_word->prob_list[i].word_struct_ptr->word,second_word->word)==0)//if the second word is there
        {
            ++(first_word->prob_list[i].occurrence);
            return TRUE;
        }
    }//the first time the second word is after the first word:
    WordProbability new_word_to_probability={second_word,1};
    if(first_word->WordProbabilityLength==0)//create a new array if the first word does not have probabilityList:
    {
        first_word->prob_list=(WordProbability*)malloc(sizeof(WordProbability));
        if (first_word->prob_list == NULL)
        {
            printf("Allocation failure: could not allocate memory");
            exit(EXIT_FAILURE);
        }
    }
    else//increase the array if the first word have probabilityList:
    {
        first_word->prob_list=(WordProbability*)realloc(first_word->prob_list,sizeof(WordProbability) *((first_word->WordProbabilityLength) +1));
        if (first_word->prob_list == NULL)
        {
            printf("Allocation failure: could not allocate memory");
            exit(EXIT_FAILURE);
        }
    }
    first_word->prob_list[first_word->WordProbabilityLength]=new_word_to_probability;//put the second word in the probabilityLIst of the first word
    ++(first_word->WordProbabilityLength);
    return FALSE;
}

/**
 * Read word from the given file. Add every unique word to the dictionary.
 * Also, at every iteration, update the prob_list of the previous word with
 * the value of the current word.
 * @param fp File pointer
 * @param words_to_read Number of words to read from file.
 *                      If value is bigger than the file's word count,
 *                      or if words_to_read == -1 than read entire file.
 * @param dictionary Empty dictionary to fill
 */
void fill_dictionary(FILE *fp, int words_to_read, LinkList *dictionary) {
    char str[MAX_SENTENCE_LENGTH + 1];
    const char s[2] = " ";
    char *token, *s_word;
    int add_flag = FALSE, i = 1;
    Node *ptr,*ptr2;
    while((fgets(str,MAX_SENTENCE_LENGTH+1,fp))!=NULL)//read the file line by line
    {
        token = strtok(str, s);//get the first word from the line
        while (token!=NULL)
        {
            if(token[strlen(token)-1]=='\n')
            {
                token[strlen(token)-1]='\0';
            }
            s_word= (char*)malloc((strlen(token)+1));
            if (s_word == NULL)
            {
                printf("Allocation failure: could not allocate memory");
                exit(EXIT_FAILURE);
            }
            strcpy(s_word,token);
            ++i;
            add_flag=FALSE;
            if(dictionary->first!=NULL)//if the dictionary is not empty:
            {
                ptr=dictionary->first;
                while (ptr!=NULL)//go over every WordStruct in the dictionary
                {
                    if (strcmp((ptr->data->word), s_word) == 0)//there is a node with this word in the dictionary
                    {
                        ++(ptr->data->textOccurrences);
                        if(ptr2!=NULL)//if this word should be in the probabilityList of the previous WordStruct
                        {
                            add_word_to_probability_list(ptr2->data, ptr->data);
                        }
                        if(s_word[strlen(s_word)-1]=='.')//if the word has a dot at the end, the next WordStruct will not be in its probabilityList
                        {
                            ptr2=NULL;
                        }
                        else//if the word has no dot at the end, the next WordStruct will be in ptr probabilityList
                        {
                            ptr2=ptr;
                        }
                        add_flag = TRUE;//update the flag that the word have been handled
                        free(s_word);
                        break;//go to handle the next word
                    }
                    ptr = ptr->next;
                }
            }
            if ((dictionary->first == NULL )||(add_flag==FALSE))//if the dictionary is empty or the word is not on the list:
            {
                WordStruct *new_word_struct = (WordStruct *) malloc(sizeof(WordStruct));//create new WordStruct with the word
                if (new_word_struct == NULL) {
                    printf("Allocation failure: could not allocate memory");
                    exit(EXIT_FAILURE);
                }
                *new_word_struct = (WordStruct) {s_word, NULL, 0,1};
                if((dictionary->first!=NULL)&&(ptr2!=NULL))//if this word should be in the probabilityList of the previous WordStruct
                {
                    add_word_to_probability_list(ptr2->data, new_word_struct);
                }
                add(dictionary,new_word_struct);
                if(s_word[strlen(s_word)-1]=='.')//if the word has a dot at the end, the next WordStruct will not be in its probabilityList
                {
                    ptr2=NULL;
                }
                else//if the word has not a dot at the end, the next WordStruct will be in dictionary->last probabilityList
                {
                    ptr2=dictionary->last;
                }
            }
            if((i>words_to_read)&&(words_to_read!=-1))//if the amount of words that have been read is equal to words_to_read
            {
                fclose(fp);
                return;
            }
            token = strtok(NULL, s);//take the next word from the line
        }
    }
    fclose(fp);
}

/**
 * Free the given dictionary and all of it's content from memory.
 * @param dictionary Dictionary to free
 */
void free_dictionary(LinkList *dictionary)
{
    Node *ptr;
    while (dictionary->first != NULL)//go over every WordStruct in the dictionary free it
    {
        ptr = dictionary->first;
        if(ptr->data->WordProbabilityLength!=0)//if the WordStruct has a probabilityList
        {
            free(ptr->data->prob_list);
        }
        free(ptr->data->word);
        free(ptr->data);
        dictionary->first = dictionary->first->next;
        free(ptr);
    }
    free(dictionary);
}

/**
 * @param argc
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 *             3) Path to file
 *             4) Optional - Number of words to read
 */
int main(int argc, char *argv[])
{
    FILE *fp;
    int i;
    if((argc!=4)&&(argc!=3))//arguments fault
    {
        printf("Usage: <tweets> <path> <optional: number of words>\n");
        exit(EXIT_FAILURE);
    }
    fp=fopen(argv[2],"r");//open the text for reading
    if(fp==NULL)
    {
        printf("Error: could not open the file");
        exit(EXIT_FAILURE);
    }
    LinkList *dictionary = (LinkList*)malloc(sizeof (LinkList));//create a new dictionary:
    if (dictionary == NULL)
    {
        printf("Allocation failure: could not allocate memory");
        exit(EXIT_FAILURE);
    }
    *dictionary=(LinkList){NULL,NULL,0};
    srand(time(NULL));
    if(argc==3)//read the all file and fill the dictionary
    {
        fill_dictionary(fp,-1,dictionary);
    }
    else //read part of the file and fill the dictionary
    {
        fill_dictionary(fp,atoi(argv[3]),dictionary);
    }
    for(i=0;i<atoi(argv[1]);i++)//print the tweets
    {
        printf("Tweet %d: ",i+1);
        generate_sentence(dictionary);
        printf("\n");
    }
    free_dictionary(dictionary);//free the dictionary from the memory
    return 0;
}
