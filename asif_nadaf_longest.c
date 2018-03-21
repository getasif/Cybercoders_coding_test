/*================================================================================================================================== 
 |   Assignment:  Find the 10 longest words from a file.
 |
 |       Author:  Asif Nadaf
 |     Language:  C
 |   To Compile:  gcc -Wall -g asif_nadaf_longest.c -o asif_nadaf_longest.o
 |                Or use the Makefile with make utility.
 |
 |       Client:  CYBERCODERS
 |
 +----------------------------------------------------------------------------------------------------------------------------------
 |
 |  Description:  Write a one-file C program. Name it (first_name)_(last_name)_longest.c.
 |                Your program should print out the 10 longest words from that file.
 |                Words can be printed in any order, and if there are multiple words tied for 10th longest you can decide what to do.
 |
 |        Input:  A file containing words, example: https://raw.githubusercontent.com/eneko/data-repository/master/data/words.txt
 |
 |       Output:  output on the screen, can be redirected to a file using executable ./asif_nadaf_longest.o > output.txt
 |                For duplicates and string lengths of same size I have created a linked list for the Hashmap key(string length).
 |                Strings are printed in descending order of string length.
 |
 |    Algorithm:  Using the C99 feature of variable sized array and Hashmap algorithm.
 |
 |   Known Bugs:  No corner cases identified as of now. Care taken to perform Garbage collection as C says its programmer's botheration.
 |
 *===========================================================================*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define FALSE 0
#define TRUE  1

#define LARGEST_RECORDED_STRING(x,y) ((x>=y)?x:y)

#define DEBUG_ENABLE_DISABLE FALSE

#if DEBUG_ENABLE_DISABLE
#define PRINT_DEBUG_STATEMENT TRUE
#endif

int g_largest_string_index = 0; /*global variable to hold the longest string length*/



struct node
{
    size_t string_length;
    struct node *link;
    char *string;    /*in the end, thinking about padding and compactness*/
};

/*Function Declarations*/
int Read_Words(const char *filename);   /*returns the total number of words in a file, words could be of any varying length*/
void Create_Hashmap_index(const char *filename, struct node* (*ptr_word_hash_table)[]);
void Add_Hashmap_entry(size_t string_length_as_index, char *string_word, struct node* (*ptr_word_hash_table)[]);
void Display_Hashmap_largest(int g_largest_string_index, struct node* (*ptr_word_hash_table)[]);
void Hashmap_destroy(struct node* (*ptr_word_hash_table)[]);


int main()
{
    int total_word_count = 0; /*variable needed to prepare the hasmap entries count*/

    static struct node *word_hash_table[0]; /*variable sized array of (pointers to struct node)*/


    char inputFilename[] = "words.txt"; /*the text file can be found in the present directory*/

    total_word_count = Read_Words(inputFilename);
    printf("total number of words in words.txt = %d \r\n", total_word_count);

    Create_Hashmap_index(inputFilename, &word_hash_table);


    printf("largest string index is %d \r\n", g_largest_string_index);

    Display_Hashmap_largest(g_largest_string_index, &word_hash_table);

    Hashmap_destroy(&word_hash_table);

    return 0;
}

int Read_Words(const char *filename)
{
    char *mode = "rt";
    FILE *fptr = fopen(filename, mode);
    int words = 0;
    char ch;
    if (fptr == NULL)
    {
        fprintf(stderr, "Can't open input file words.txt for parsing!\n");
        exit(1);
    } else
    {
        /*counting character by character to avoid error: stack smashing detected */
        /*this way the function would be able to handle words of any length and would not 
          be dependent on the buffer size needed to hold in fscanf etc...*/
        while ((ch = fgetc(fptr)) != EOF)
        {
            if (ch == ' ' || ch == '\n')
            {
                words++;
            }
        }
    }
    return words;
}

void Create_Hashmap_index(const char *filename, struct node* (*ptr_word_hash_table)[])
{
    char *mode = "rt";
    FILE *fptr = fopen(filename, mode);
    int character = 0;
    char *string_word = NULL;

    int string_length = 0;
    int ch;
    if (fptr == NULL)
    {
        fprintf(stderr, "Can't open input file words.txt for parsing!\n");
        exit(1);
    } 
    else
    {
        while (ch != EOF)
        {
            ch = fgetc(fptr);
            ++character;
            if (ch == ' ' || ch == '\n')
            {
                string_length = character - 1;  /*beware and consider the ' ' or '\n', replace with 0 */
                if(NULL == (string_word = (char *)malloc(string_length + 2)))/*adding 1 for the null terminating char */
                {
                            perror("Memory allocation failed");
                            exit(1);
                }
                fseek(fptr, -(string_length + 1), SEEK_CUR);

                fgets(string_word, string_length + 2, fptr); /*advances to the next word entry*/
                if (string_word[string_length] == '\n')
                {
                    string_word[string_length] = '\0';
                }

                /*here string_length doesn't include the last null terminating char*/
#if defined PRINT_DEBUG_STATEMENT
                printf("the word in str is %s and length = %d \n", string_word, string_length);
#endif
                Add_Hashmap_entry(string_length, string_word, ptr_word_hash_table);
                free(string_word); /*code cleanup for local temporary string variable*/
                character = string_length = 0;
            }
        }
    }
}


void Add_Hashmap_entry(size_t string_length_as_index, char *string_word, struct node* (*ptr_word_hash_table)[])
{
    struct node *current_node, *new_node, *temp;
    temp = current_node = (*ptr_word_hash_table)[string_length_as_index];


    /*populate the string length and string variables in the node*/
    if(NULL == (new_node = (struct node *)malloc(sizeof(struct node)))) /*adding 1 for the null terminating char */
    {
                perror("Memory allocation failed ");
                exit(1);
    }
    new_node->string_length = string_length_as_index;
    if(NULL == (new_node->string = (char *)malloc(string_length_as_index + 1))) /* 1 char more for '\0' */
    {
                perror("Memory allocation failed ");
                exit(1);
    }
    strcpy(new_node->string, string_word);
    new_node->link = NULL;

#if defined PRINT_DEBUG_STATEMENT
    printf("index is %lu and word is %s\r\n", string_length_as_index, string_word);
#endif

    /* if list is empty */
    if (current_node == NULL)
    {
        (*ptr_word_hash_table)[string_length_as_index] = current_node = new_node;
    } else
    {
        /* traverse the list till the end to add the new node with equivalent string length*/
        while (temp != NULL)
        {
            if (temp->link == NULL)
            {
                temp->link = new_node;
                break;
            }
            temp = temp->link;
        }
    }

    g_largest_string_index = LARGEST_RECORDED_STRING(g_largest_string_index, string_length_as_index);
}


void Display_Hashmap_largest(int g_largest_string_index, struct node* (*ptr_word_hash_table)[])
{
    int largest_index_temp = g_largest_string_index; /*lets leave global variable untouched*/

    int i = 10; struct node *temp;

    while (i <= 10 && i != 0)
    {
        if ((*ptr_word_hash_table)[largest_index_temp] != NULL)
        {
            temp = (*ptr_word_hash_table)[largest_index_temp];
            while (temp != NULL)
            {
                printf("%4d) Largest string is %-s\t with string length=%2ld \r\n", i, temp->string, temp->string_length);
                temp = temp->link;
            }
        } else
        {
            largest_index_temp--;
            continue;
        }
        i--;
        largest_index_temp--;
    }
}


/*function to do code clean up, possibly give menu interface in main and default option of code cleanup*/

void Hashmap_destroy(struct node* (*ptr_word_hash_table)[])
{
    int largest_index_temp = g_largest_string_index; /*lets leave global variable untouched*/
    int i = largest_index_temp;

    struct node *head_reference = NULL;
    struct node *next_node = NULL;
    struct node *current_node = NULL;

    while (((*ptr_word_hash_table)[largest_index_temp] != NULL) && (i <= largest_index_temp && i != 0))
    {
        current_node = head_reference = (*ptr_word_hash_table)[i];
        while (current_node != NULL)
        {
#if defined PRINT_DEBUG_STATEMENT
            printf("node with string %s deleted \r\n", current_node->string);
#endif
            next_node = current_node->link;
            free(current_node->string);  /*free the malloced string*/
            free(current_node);          /*finally free the whole node*/
            current_node = next_node;
        }

        head_reference = NULL;
        i--;
    }
}
