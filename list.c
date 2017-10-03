
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "list.h"

typedef struct link link_t;

struct list {
  link_t *first;
  link_t *last;
};

struct link {
  L value;
  link_t *next;
};

/**
 *  Create new list.
 */
list_t *list_new()
{
  return calloc(1, sizeof(list_t));
}

/**
 * Adds item to end of list.
 */
void list_append(list_t *list, L elem)
{
  link_t *new = calloc(1, sizeof(link_t));
  new->value = elem;

  if (list->last)
    {
      list->last->next = new;
    }
  else
    {
      // If last is not set, neither is first
      list->first = new;
    }

  list->last = new;
}

/**
 * Adds item to beginning of list.
 */
void list_prepend(list_t *list, L elem)
{
  link_t *new = calloc(1, sizeof(link_t));
  new->value = elem;

  if (list->first)
    {
      new->next = list->first;
    }
  else
    {
      // If first is not set, neither is last
      list->last = new;
    }

  list->first = new;
}

/**
 * Inserts item in list at index.
 */
bool list_insert(list_t *list, int index, L elem)
{
  int size = list_length(list);

  // Make sure index is in range [-size, size]
  if (index < size * -1 || index > size)
    {
      return false;
    }
  else
    {
      link_t *new = calloc(1, sizeof(link_t));
      new->value = elem;

      if (size == 0)
	{
	  list->first = new;
	  list->last = new;
	}
      else
	{
	  // Handle negative indices:
	  // list l = {a, b, c, d}, size = 4, insert x at -2; -2 + 4 = 2 => {a, b, x, c, d}
	  int pos = index < 0 ? index + size : index;
	  link_t *cursor = list->first;

	  if (pos == 0)
	    {
	      list_prepend(list, elem);
	    }
	  else 
	    {
	      // Move cursor to index
	      for (int i = 1; i < pos; i++)
		{
		  cursor = cursor->next;
		}

	      // Insert item
	      link_t *tmp = cursor->next;
	      cursor->next = new;
	      new->next = tmp;
	    }
	}

      return true;
    }
}

/**
 * Removes item at index in list and save value at elem.
 */
bool list_remove(list_t *list, int index, L *elem)
{
  // Link to remove
  link_t *rem = list->first;

  int size = list_length(list);
  int pos = index < 0 ? index + size - 1 : index;

  if (size == 0 || pos < 0 || pos > size - 1)
    {
      // Out of bounds
      return false;
    }
  else if (pos == 0)
    {
      // Removing last list item
      if (size == 1)
	{
	  list->first = NULL;
	  list->last = NULL;
	}
      else
	{
	  list->first = list->first->next;
	}
    }
  else
    {
      link_t *prev;
      for (int i = 0; i < pos; ++i)
	{
	  prev = rem;
	  rem = rem->next;
	}
      
      prev->next = rem->next;
    }
  
  if (elem != NULL) elem = rem->value;
  
  free(rem);
  return true;
}

/**
 * Returns value at index in list.
 */
L list_get(list_t *list, int index)
{
  link_t *cursor = list->first;

  for (int i = 0; cursor && i < index; ++i)
    {
      cursor = cursor->next;
    }

  return cursor->value;
}

/**
 * Shorthand for list_get(list, 0);
 */
L list_first(list_t *list)
{
  return list->first->value;  
}

/**
 * Shorthand for list_get(list, -1);
 */
L list_last(list_t *list)
{
  return list->last->value;  
}

/**
 * Returns length of list.
 */
int list_length(list_t *list)
{
  link_t *cursor = list->first;
  int count = 0;

  while (cursor != NULL)
    {
      cursor = cursor->next;
      ++count;
    }

  return count;
}
