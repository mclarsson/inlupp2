#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "list.h"

struct list
{
  link_t *first;
  link_t *last;
};
struct link
{
  L value;
  link_t *next;
};



/// Allocates memory for/and creates a list
///
/// \returns: empty list
list_t *list_new()
{
  return calloc(1, sizeof(list_t));
}


/// Creates a link between current value and next link
///
/// \param value of new element
/// \param next next link in line
/// \returns: link, part of a list
link_t *link_new(L value, link_t *next)
{
  link_t *new = calloc(1, sizeof(link_t));

  if (new != NULL)
    {
      new -> value = value;
      new -> next = next;
    
    }
  return new;
}



/// Adds an item elem to the end of a list
///
/// \param list list which is appended upon
/// \param elem element that is added to the list
void list_append(list_t *list, L elem)
{
  link_t *new = link_new(elem, NULL);
  if (list->first == NULL)
    {
      list->first = new;
      list->last = new;
    }
  else
    {
      list->last->next = new;
      list->last = new;
    }

}


/// Adds an item elem to the beginning of a list
///
/// \param list list which is prepended upon
/// \param elem element that is added to the list
void list_prepend(list_t *list, L elem)
{
  list->first = link_new(elem, list->first);
  if (list->last == NULL)
    {
      list->last = list->first;
    }
}



/// Returns the value of the element located at point of index
///
/// \param list list that is searched for value
/// \param index indicator for which element to return
/// \returns: pointer to whatever value is stored at index
L *list_get(list_t *list, int index)
{
  link_t *cursor = list->first;
  for (int i = 0; cursor->next != NULL && i < index; i++)
    {
      cursor = cursor->next;
    }
  return cursor->value;
}


/// Adds an item elem at the point of index in a list
///
/// \param list the list
/// \param index point of the list where the element is added
/// \param elem element to be added to list
/// \returns: true if successful, else false
bool list_insert(list_t *list, int index, L elem)
{
  
  // At the start of the list
  if (index == 0)
    {
      list_prepend(list, elem);
      return true;
    }

  // At the end of the list
  else if (index == list_length(list))
    {
      list_append(list, elem);
      return true;
    }

  // Out of bounds (index too small)
  else if (index < 0)
      {
        return list_insert(list, index + list_length(list), elem);
      }

  // Out of bounds (index too large)
  else if (index > list_length(list))
    {
      puts("Not a valid index.\n");
      return false;
    }

  
  
  link_t *cursor = list->first;
  for (int i = 0; cursor != NULL && i < index-1; ++i)
    {
      cursor = cursor->next;
    }
  
  link_t *tmp = cursor->next;
  link_t *new_link = link_new(elem, tmp);
  cursor->next = new_link;
  
  return true;

}




/// Get the length of a list
///
/// \param list the list
/// \returns: length of the list
int list_length(list_t *list)
{
  int counter = 0;
  link_t *cursor = list->first;
  while (cursor != NULL)
    {
      cursor = cursor->next;
      counter = counter + 1;
    }
  return counter;
}



/// Removes an element at the point of index
///
/// \param list the list
/// \param index point of the list indicating the element
/// \param elem element to be removed
/// \returns: true if successful, else false
bool list_remove(list_t *list, int index, L *elem)
{
  // Link to remove
  link_t *remove = list->first;

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
	  prev = remove;
	  remove = remove->next;
	}
      
      prev->next = remove->next;
    }
  
  if (elem != NULL) elem = remove->value;
  
  free(remove);
  return true;
}


/// Get the value of the first element in a list
///
/// \param list the list
/// \returns: pointer to the first value in list
L *list_first(list_t *list)
{
  return list->first->value;
}



/// Get the value of the last element in a list
///
/// \param list the list
/// \returns: pointer to the last value in list
L *list_last(list_t *list)
{
  return list->last->value;
}



