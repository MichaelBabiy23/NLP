#include "markov_chain.h"
#include <stdio.h>
#include <string.h>


/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number)
{
    if (max_number <= 0)
        return 0;
    return rand() % max_number;
}

Node* get_node_from_database(MarkovChain *markov_chain, char *data_ptr)
{
    // check if markov_chain is empty
    if (markov_chain == NULL || markov_chain->database->first == NULL)
        return NULL;

    // go through the chain and check for data_ptr value
    Node* checkNode = markov_chain->database->first;
    while (checkNode != NULL && strcmp(checkNode->data->data, data_ptr) != 0)
        checkNode = checkNode->next;

    return checkNode;
}

Node* add_to_database(MarkovChain *markov_chain, char *data_ptr)
{
    // check if the node already exists
    Node* checkNode = get_node_from_database(markov_chain, data_ptr);
    if (checkNode != NULL)
        return checkNode;

    // create new markov node that will be added through 'add' function of the linked list
    MarkovNode *newMarkovNode = (MarkovNode*) malloc (sizeof (MarkovNode));
    if (newMarkovNode == NULL)
        return NULL;
    newMarkovNode->frequency_list = NULL;
    newMarkovNode->last_node_frequency_list = NULL;
    newMarkovNode->data = strdup(data_ptr); // TODO: 'strdup' is needed?

    // check if malloc is failed to allocate new space for the new node
    if (add(markov_chain->database, newMarkovNode) ==  1)
    {
        free (newMarkovNode);
        return NULL;
    }

    // return the last added node
    return markov_chain->database->last;
}

int add_node_to_frequency_list(MarkovNode *first_node, MarkovNode *second_node)
{
    // check if frequency list of the first node is empty
    MarkovNodeFrequency* frequency_list = first_node->frequency_list;
    if (frequency_list == NULL)
    {
        // add to first
        first_node->frequency_list = (MarkovNodeFrequency*) malloc (sizeof (MarkovNodeFrequency));
        // if failed to allocate memory
        if (first_node->frequency_list == NULL)
        {
            fprintf (stderr, ALLOCATION_ERROR_MASSAGE);
            return EXIT_FAILURE;
        }

        // add the new data to the node
        first_node->frequency_list->markov_node = second_node;
        first_node->frequency_list->frequency = 1;
        first_node->last_node_frequency_list = first_node->frequency_list;
        first_node->frequency_list->next = NULL;
    }
    else
    {
        // check if the second node already in the frequency list
        MarkovNodeFrequency *temp = first_node->frequency_list;
        while (temp != NULL && temp->markov_node != second_node)
            temp = temp->next;

        // not in list, add to last
        if (temp == NULL)
        {
            first_node->last_node_frequency_list->next = (MarkovNodeFrequency*) malloc (sizeof (MarkovNodeFrequency));
            if (first_node->last_node_frequency_list->next == NULL)
            {
                fprintf (stderr, ALLOCATION_ERROR_MASSAGE);
                return EXIT_FAILURE;
            }

            // add the new data to the node
            first_node->last_node_frequency_list->next->markov_node = second_node;
            first_node->last_node_frequency_list->next->frequency = 1;
            first_node->last_node_frequency_list = first_node->last_node_frequency_list->next;
            first_node->last_node_frequency_list->next = NULL;

        }
        else
        {
            // update existing node
            temp->frequency++;
        }
    }
    return 0;
}



void free_database(MarkovChain ** ptr_chain)
{
    // pointer to head of the node chain
    Node *head = (*ptr_chain)->database->first;
    // check if head is not null
    while (head != NULL)
    {
        // store the next pointer in order to free the previous one
        Node *next = head->next;
        // free the markov node inside
        free_MarkovNode(head->data);
        // free the head itself
        free(head);
        // move to the next node
        head = next;
    }
    // free the linked list
    free((*ptr_chain)->database);
    // free the markov chain
    free(*ptr_chain);
}

void free_MarkovNode(MarkovNode *head)
{
    if (head != NULL)
    {
        // free all head content from memory
        // the word
        free(head->data);
        free_MarkovNodeFrequency(head->frequency_list);
        free(head);
    }
}

void free_MarkovNodeFrequency(MarkovNodeFrequency *head)
{
    // loop through all the nodes and free each of them
    while (head != NULL)
    {
        MarkovNodeFrequency *next = head->next;
        free(head);
        head = next;
    }
}

MarkovNode* get_first_random_node(MarkovChain *markov_chain)
{
    // generate random number [0, size of chain)
    int randomNumber = get_random_number(markov_chain->database->size);
    Node *head = markov_chain->database->first;
    // loop through all the nodes and subtract one from the random number
    while (randomNumber > 0)
    {
        head = head->next;
        randomNumber--;
    }
    return head->data;
}

MarkovNode* get_next_random_node(MarkovNode *cur_markov_node)
{
    // accumulate of all the frequencies
    int sumOfFrequencies = 0;
    MarkovNodeFrequency *cur_frequency_list = cur_markov_node->frequency_list;
    // loop through all the nodes and sum their frequency
    while (cur_frequency_list != NULL)
    {
        sumOfFrequencies += cur_frequency_list->frequency;
        cur_frequency_list = cur_frequency_list->next;
    }
    // generate a random number
    int randomFrequency = get_random_number(sumOfFrequencies);
    cur_frequency_list = cur_markov_node->frequency_list;
    // loop through all the nodes and subtract their frequency
    while (cur_frequency_list != NULL)
    {
        randomFrequency -= cur_frequency_list->frequency;
        if (randomFrequency < 0)
            break;
        cur_frequency_list = cur_frequency_list->next;

    }

    return cur_frequency_list->markov_node;
}

void generate_tweet(MarkovNode *first_node, int max_length)
{
    for (int i = 0; i < max_length - 1; i++)
    {
        // if the word ends with dot, print it and stop the tweet generation
        if (first_node->data[strlen(first_node->data) - 1] == '.')
        {
            fprintf(stdout, "%s\n", first_node->data);
            return;
        }
        // print the word with space after it
        fprintf(stdout, "%s ",first_node->data);
        // go to the next random node
        first_node = get_next_random_node(first_node);
    }
    fprintf(stdout, "%s\n",first_node->data);
}


