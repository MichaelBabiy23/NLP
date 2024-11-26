#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "markov_chain.h"

#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

// we asuume that the max words per sentence is 20
#define MAX_WORDS_PER_SENTENCE 20


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
    return EXIT_FAILURE;
}

bool not_end_of_file(char *buffer, FILE *fp)
{
    return fgets(buffer, MAX_LINE_LENGTH, fp) != NULL;
}

bool is_word_ends_with_dot(char *word)
{
    return word[strlen(word) - 1] == dot_char;
}
int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain) {
    char buffer[MAX_LINE_LENGTH];
    char *token;
    Node *first_node = NULL;
    Node *second_node = NULL;

    while (not_end_of_file(buffer, fp) && still_have_to_read(words_to_read)) {
        // move to the first word on the sentence
        token = strtok(buffer, DELIMITERS);
        if (token == NULL) {
            continue;
        }

        first_node = add_to_database(markov_chain, token);
        if (first_node == NULL) {
            return show_ALLOCATION_ERROR_MASSAGE_free_and_return(markov_chain);
        }

        // move to the next word
        token = strtok(NULL, DELIMITERS);
        words_to_read--;
        while (token != NULL && still_have_to_read(words_to_read)) {
            second_node = add_to_database(markov_chain, token);
            if (second_node == NULL) {
                return show_ALLOCATION_ERROR_MASSAGE_free_and_return(markov_chain);
            }

            if (is_word_ends_with_dot(second_node->data->data) == false) {
                add_node_to_frequency_list(first_node->data, second_node->data);
                // continue the sentence
                // replace the first node with the second one to move forward and keep the last node
                first_node = second_node;
            } else if (still_have_to_read(words_to_read)) {
                add_node_to_frequency_list(first_node->data, second_node->data);
                // start a new sentence, dot found
                first_node = add_to_database(markov_chain, token);
                if (first_node == NULL) {
                    return show_ALLOCATION_ERROR_MASSAGE_free_and_return(markov_chain);
                }
            }

            // move to the next word
            token = strtok(NULL, DELIMITERS);
            words_to_read--;
        }
    }

    return 0;
}


int main(int argc, char **argv)
{
    if (argc != 4 && argc != 5)
    {
        fprintf(stdout, NUM_ARGS_ERROR);
        return EXIT_FAILURE;
    }
    int seed = atoi(argv[1]);
    srand(seed);
    int numOfTweets = atoi(argv[2]);
    FILE *file = fopen(argv[3], "r");
    if (file == NULL)
    {
        fprintf(stdout, FILE_PATH_ERROR);
        return EXIT_FAILURE;
    }
    MarkovChain *markov_chain = (MarkovChain *)malloc(sizeof(MarkovChain));
    if (markov_chain == NULL)
    {
        // TODO : free all
        fprintf(stdout, ALLOCATION_ERROR_MASSAGE);
        return EXIT_FAILURE;
    }
    markov_chain->database = (LinkedList *)malloc(sizeof(LinkedList));
    if (markov_chain->database == NULL)
    {
        // TODO : free all
        fprintf(stdout, ALLOCATION_ERROR_MASSAGE);
        return EXIT_FAILURE;
    }
    markov_chain->database->first = NULL;
    markov_chain->database->last = NULL;
    markov_chain->database->size = 0;
    bool fill_success = 1;
    if (argc == 5)
    {
        int words_to_read = atoi(argv[4]);
        fill_success = fill_database(file, words_to_read, markov_chain);
    }
    else
        fill_success = fill_database(file, -1, markov_chain);

    if (fill_success != 0)
    {
        fprintf(stdout, "Fill database failed");
        return EXIT_FAILURE;
    }

    for (int i = 1; i <= numOfTweets; i++)
    {
        fprintf(stdout, "Tweet %d: ", i);
        MarkovNode *markovNode = get_first_random_node(markov_chain);
        while (markovNode == NULL || is_word_ends_with_dot(markovNode->data))
            markovNode = get_first_random_node(markov_chain);
        generate_tweet(markovNode, MAX_WORDS_PER_SENTENCE);
    }

    return EXIT_SUCCESS;
}




