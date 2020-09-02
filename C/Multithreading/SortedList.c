//NAME: OMAR AHMAD
//EMAIL: omarahmad4@uclae.edu
//ID: 8048010272



/*
  struct SortedListElement {
  struct SortedListElement *prev;
  struct SortedListElement *next;
  const char *key;
  };
*/


#include "SortedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>

typedef struct SortedListElement SortedList_t;
typedef struct SortedListElement SortedListElement_t;

extern int opt_yield;
#define	INSERT_YIELD	0x01	// yield in insert critical section
#define	DELETE_YIELD	0x02	// yield in delete critical section
#define	LOOKUP_YIELD	0x04	// yield in lookup/length critical esction

/**
 * SortedList_insert ... insert an element into a sorted list
 *
 *	The specified element will be inserted in to
 *	the specified list, which will be kept sorted
 *	in ascending order based on associated keys
 *
 * @param SortedList_t *list ... header for the list
 * @param SortedListElement_t *element ... element to be added to the list
 */
void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
  //  printf("length is %i\n", SortedList_length(list));
  if(list->next == list)// || SortedList_length(list) == 0) //empty list
    {
      //printf("enter empty list section\n");
      list->next = element;
      list->prev = element;
      element->next = list;
      element->prev = list;
      return;
    }
  //printf("didn't emter empty list section\n");
  
  SortedList_t * curr = list;
  SortedList_t * nextElem = list->next;
  /*   if(nextElem == curr) //if empty list
    {
      //printf("eneter second empty list seciont\n");
      curr->next = element;
      curr->prev = element;
      element->next = curr;
      element->prev= curr;
      return;
    }
  */
  do
    {
      curr = nextElem;
      nextElem = curr->next;
    }
  while(element->key < curr->key); //getting into correct pos
  //now in correct position
  curr->next = element;
  element->next = nextElem;
  if(opt_yield && INSERT_YIELD)
    sched_yield();
  element->prev = curr;
  nextElem->prev = element;
}

/**
 * SortedList_delete ... remove an element from a sorted list
 *
 *	The specified element will be removed from whatever
 *	list it is currently in.
 *
 *	Before doing the deletion, we check to make sure that
 *	next->prev and prev->next both point to this node
 *
 * @param SortedListElement_t *element ... element to be removed
 *
 * @return 0: element deleted successfully, 1: corrtuped prev/next pointers
 *
 */
int SortedList_delete( SortedListElement_t *element)
{
  if(element-> prev == NULL || element->next == NULL)
    return -1;
     
  
  SortedList_t * prevElem = element->prev;
  SortedList_t * nextElem = element->next;
  
  if(nextElem->prev == NULL || nextElem-> prev != element || prevElem->next == NULL || prevElem->next != element)
    return 1;
  else
    {
      prevElem->next = nextElem;
      if(opt_yield && DELETE_YIELD)
	sched_yield();

      nextElem->prev = prevElem;
    }
  //  free(element);
  return 0;
}

/**
 * SortedList_lookup ... search sorted list for a key
 *
 *	The specified list will be searched for an
 *	element with the specified key.
 *
 * @param SortedList_t *list ... header for the list
 * @param const char * key ... the desired key
 *
 * @return pointer to matching element, or NULL if none is found
 */
SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
  SortedList_t * curr = list;
  SortedList_t * nextElem = curr->next;
  while(nextElem != list)
    {
      if(*nextElem->key == *key)
	return nextElem;
      curr = nextElem;
      if(opt_yield && LOOKUP_YIELD)
	sched_yield();
      nextElem = curr->next;
    }
  return NULL; //if key not found

}


/**
 * SortedList_length ... count elements in a sorted list
 *	While enumeratign list, it checks all prev/next pointers
 *
 * @param SortedList_t *list ... header for the list
 *
 * @return int number of elements in list (excluding head)
 *	   -1 if the list is corrupted
 */

int SortedList_length(SortedList_t *list)
{
  int counter = 0;
  SortedList_t * curr = list;
  SortedList_t * nextElem = curr->next;
  if(curr->next == curr) //empty list
    return 0;
  if(nextElem == NULL)
    {
      //printf("next elem = NULL\n");
      return -1;
    }
  if(nextElem->prev != curr)
    {
      //printf("nextelem's prev != curr\n");
      return -1;
    }
  while(nextElem != list) //circular list so at end points back to head
    {
      curr = nextElem;
      if(opt_yield && LOOKUP_YIELD)
	sched_yield();

      nextElem = nextElem->next;
      if(nextElem == NULL) //broken forward pointer -- corrupted 
	{
	  //printf("broken forward pointer\n");
	  return -1;
	}
      if(nextElem->prev != curr) //broken prev pointer -- corrupted
	{
	  //printf("broken prev pointer\n");
	  return -1;
	}
      counter++;
    }
  return counter;
}

