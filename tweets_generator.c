#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "markov_chain.h"

#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

// we assume that the max size of each line is 1000
#define MAX_LINE_LENGTH 1000

#define dot_char '.'

#define DELIMITERS " \n\t\r"

bool still_have_to_read(int words_to_read)
{
  return (words_to_read > 0 || words_to_read <= -1);
}

int show_ALLOCATION_ERROR_MASSAGE_free_and_return(MarkovChain *markov_chain)
{
    free(markov_chain);
    fprintf(stdout, ALLOCATION_ERROR_MASSAGE);
    return 1;
}

bool not_end_of_file(char *buffer, FILE *fp)
{
    return fgets(buffer, MAX_LINE_LENGTH, fp) != NULL;
}

int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain)
{
    char buffer[MAX_LINE_LENGTH];
    // words_to_read == -1 means, read whole file
    while (not_end_of_file(buffer, fp) && still_have_to_read(words_to_read))
    {
        // move to the first word
        char *token = strtok(buffer, DELIMITERS);
        Node *first_node = add_to_database(markov_chain, token);
        // allocation failed
        if (first_node == NULL)
            return show_ALLOCATION_ERROR_MASSAGE_free_and_return(markov_chain);
        //word read
        words_to_read--;

        while (token != NULL && still_have_to_read(words_to_read))
        {
            // move to the next word
            token = strtok(NULL, DELIMITERS);
            Node *second_node = add_to_database(markov_chain, token);
            // allocation failed
            if (second_node == NULL)
                return show_ALLOCATION_ERROR_MASSAGE_free_and_return(markov_chain);
            //word read
            words_to_read--;
            // not end of sentence
            if (second_node->data->data[strlen(second_node->data->data) - 1] != dot_char)
            {
                add_node_to_frequency_list(first_node->data, second_node->data);
                // continue the sentence
                // replace the first node with the second one to move forward and keep the last node
                first_node = second_node;
            }
            // end of sentence
            else if (still_have_to_read(words_to_read))
            {
                // move to the next word
                token = strtok(NULL, DELIMITERS);
                // start a new sentence, dot found
                first_node = add_to_database(markov_chain, token);
                // word read
                words_to_read--;
            }
        }
    }
    return 0;
}



int main(int argc, char **argv)
{
    if (argc != 4 || argc != 5)
        fprintf(stdout, NUM_ARGS_ERROR);

    FILE *file = fopen(argv[2], "r");
    if (file == NULL)
      fprintf(stdout, FILE_PATH_ERROR);

    MarkovChain *markov_chain;
    if (argc == 5)
        return fill_database(file, (int) argv[4], markov_chain);
    return fill_database(file, -1, markov_chain);
}




