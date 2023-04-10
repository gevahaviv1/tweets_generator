#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "markov_chain.h"
#include "linked_list.h"
#define MAX_LINE 1000
#define INPUT_1 5
#define INPUT_2 4
#define MAX_WORDS_IN_TWEET 20

/**
 * Create and allocate new memory for markov chain and linked list.
 * Also initialize them.
 * @return Pointer from type MarkovChain.
 */
MarkovChain *initialize_markov_chain()
{
  MarkovChain *markov_chain = malloc (sizeof (MarkovChain));
  if (!markov_chain) return NULL;

  LinkedList *list = malloc (sizeof (LinkedList));
  if (!list)
  {
    free (markov_chain);
    markov_chain = NULL;
    return NULL;
  }

  list->first = NULL;
  list->last = NULL;
  list->size = 0;
  markov_chain->list = list;
  return markov_chain;
}

/**
 * Check if the word is already exist in the markov chain.
 * @param markov_chain the chain we search on.
 * @param word the word we search.
 * @return True \ False value.
 */
bool word_in_chain(MarkovChain *markov_chain, char *word)
{
  Node *p = markov_chain->list->first;
  for (int i = 0; i < markov_chain->list->size; ++i)
  {
    if (!strcmp (word, p->data->data)) return true;
    p = p->next;
  }
  return false;
}

/**
 * Remove spaces from the word and clean it from ' ' or '\n'.
 * @param word the word we want to remove spaces from her.
 * @return Pointer to the word after the removing.
 */
char *remove_spaces(char *word)
{
  char *end;
  while(isspace((unsigned char)*word))
  {
    word++;
  }

  if(*word == 0)
  {
    return word;
  }

  end = word + strlen(word) - 1;
  while(end > word && isspace((unsigned char)*end))
  {
    end--;
  }

  end[1] = '\0';
  return word;
}


int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain)
{
  Node *p;
  Node *current;
  Node *prev;
  char line[MAX_LINE];
  char *word;
  size_t len;

  while (fgets (line, MAX_LINE, fp))
  {
    word = strtok (line, " ");
    while (word)
    {
      word = remove_spaces (word);
      if (markov_chain->list->size == 0)
      {
        markov_chain->list->first = add_to_database (markov_chain, word);
        p = markov_chain->list->first;
      }
      else if (word_in_chain(markov_chain, word))
      {
        len = strlen (prev->data->data) - 1;
        current = get_node_from_database (markov_chain, word);
        if (p && strcmp (&prev->data->data[len], ".") != 0)
          add_node_to_counter_list (prev->data, current->data);
        prev = current;
        word = strtok (NULL, " ");
        continue;
      }
      else
      {
        p->next = add_to_database (markov_chain, word);
        p = p->next;
        len = strlen (prev->data->data) - 1;
        if (p && strcmp (&prev->data->data[len], ".") != 0)
          add_node_to_counter_list (prev->data, p->data);
      }

      if (!p)
      {
        return 0;
      }
      prev = p;
      markov_chain->list->size++;
      if (markov_chain->list->size == words_to_read)
      {
        return 1;
      }
      word = strtok (NULL, " ");
    }
  }
  return 1;
}

/**
 * Write tweets function.
 * @param markov_chain the data struct we work on.
 * @param num_of_tweets the number of tweets we want to write.
 */
void write_tweets(MarkovChain *markov_chain, long num_of_tweets)
{
  MarkovNode *word;
  int num_of_words_in_tweet;
  for (int i = 1; i <= num_of_tweets; ++i)
  {
    word = get_first_random_node (markov_chain);
    fprintf (stdout, "Tweet %d: %s", i, word->data);
    num_of_words_in_tweet = 1;

    while (strcmp(word->data + strlen(word->data) - 1, ".") != 0 &&
           num_of_words_in_tweet < MAX_WORDS_IN_TWEET)
    {
      word = get_next_random_node (word);
      fprintf (stdout, " %s", word->data);
      num_of_words_in_tweet++;
    }

    fprintf (stdout, "\n");
  }
}

int main(int argc, char *argv[]){
  if ((argc != INPUT_1) && (argc != INPUT_2))
  {
    fprintf (stdout, "Usage:Something went wrong.\n"
                     "The parameters that needed:\n"
                     "1)Seed value.\n"
                     "2)Num of tweets.\n"
                     "3)Path file.\n"
                     "4)Number of words to read from the path file.");
    return EXIT_FAILURE;
  }

  FILE *fp = fopen (argv[3], "r");
  if (!fp)
  {
    fprintf (stdout, "Error:The path is not working.");
    return EXIT_FAILURE;
  }

  srand ((unsigned int)strtol(argv[1], NULL, 10));
  MarkovChain *markov_chain = initialize_markov_chain ();
  if (!markov_chain)
  {
    fprintf (stdout, ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  int words_to_read = - 1;
  if (argv[4]) words_to_read = strtol (argv[4], NULL, 10);
  if (!fill_database (fp, words_to_read, markov_chain))
  {
    fprintf (stdout, ALLOCATION_ERROR_MASSAGE);
    free_markov_chain (&markov_chain);
    return EXIT_FAILURE;
  }

  write_tweets (markov_chain, strtol(argv[2], NULL, 10));

  fclose (fp);
  free_markov_chain (&markov_chain);
  return EXIT_SUCCESS;
}