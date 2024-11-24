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
    return rand() % max_number;
}

Node* get_node_from_database(MarkovChain *markov_chain, char *data_ptr)
{
    // check if markov_chain is empty
    if (markov_chain->database->first == NULL)
        return NULL;

    // go through the chain and check for data_ptr value
    Node* checkNode = markov_chain->database->first;
    while (checkNode != NULL && !strcmp(checkNode->data->data, data_ptr))
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
    newMarkovNode->data = data_ptr; // TODO: 'strdup' is needed?

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
            return 1;

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
                return 1;

            // add the new data to the node
            first_node->last_node_frequency_list->next->next = NULL;
            first_node->last_node_frequency_list->next->markov_node = second_node;
            first_node->last_node_frequency_list->next->frequency = 1;
        }
        else
        {
            // update existing node
            temp->frequency++;
        }
        return 0;
    }






}