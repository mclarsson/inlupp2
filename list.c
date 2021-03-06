#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "list.h"

typedef struct link link_t;

/// Struct for list
struct list {
  link_t *first;
  link_t *last;
  element_comp_fun cmp_f;
  element_copy_fun copy_f;
  element_free_fun clean_f;
};


/// Struct for link-content
struct link
{
  elem_t value;
  link_t *next;
};


/// Allocates memory for/and creates a list
///
/// \param copy_fun copy function to be used to copy lists
/// \param clean_fun delete function to be used to free all parts of a list
/// \param comp_fun compare function to be used for comparing two elements of list_value_t type
/// \returns: empty list
list_t *list_new(element_copy_fun copy_fun, element_free_fun clean_fun, element_comp_fun comp_fun)
{
  list_t *new = calloc(1, sizeof(list_t));
  if (new)
    {
      new->copy_f = copy_fun;
      new->clean_f = clean_fun;
      new->cmp_f = comp_fun;
    }
  
  return new;
}


/// Creates a link between current value and next link
///
/// \param value of new element
/// \param next next link in line
/// \returns: link, part of a list
link_t *link_new(elem_t value, link_t *next)
{
  link_t *new = calloc(1, sizeof(link_t));

  if (new != NULL)
    {
      new->value = value;
      new->next = next;
    
    }
  return new;
}



/// Adds an item elem to the end of a list
///
/// \param list list which is appended upon
/// \param elem element that is added to the list
void list_append(list_t *list, elem_t elem)
{
  link_t *new = link_new(elem, NULL);
  
  // If this is an empty list
  if (list->first == NULL)
    {
      list->first = new;
      list->last = new;
    }
  
  // If the element is to be added to a non-empty list
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
void list_prepend(list_t *list, elem_t elem)
{
  list->first = link_new(elem, list->first);

  // If this is an empty list
  if (list->last == NULL)
    {
      list->last = list->first;
    }
}



/// Returns the value of the element located at point of index
///
/// \param list list that is searched for value
/// \param index indicator for which element to return
/// \param result the element of which the result is designated to return as
/// \returns: boolean determining if it was a success or not, also a pointer
///           to the result element
bool list_get(list_t *list, int index, elem_t *result)
{
  link_t *cursor = list->first;
  int pos = index < 0 ? index + list_length(list) : index;

  if (pos < 0 || pos > list_length(list)) return false;
  
  for (int i = 0; cursor->next != NULL && i < pos; i++)
    {
      cursor = cursor->next;
    }
  
  *result = cursor->value;

  return true;
}


/// Adds an item elem at the point of index in a list
///
/// \param list the list
/// \param index point of the list where the element is added
/// \param elem element to be added to list
void list_insert(list_t *list, int index,  elem_t elem)
{
  
  // At the start of the list
  if (index == 0)
    {
      list_prepend(list, elem);
    }

  // At the end of the list
  else if (index == list_length(list))
    {
      list_append(list, elem);
    }

  // Out of bounds (index too small)
  else if (index < 0)
      {
        list_insert(list, index + list_length(list), elem);
      }

  // Out of bounds (index too large)
  else if (index > list_length(list))
    {
      puts("Not a valid index.\n");
    }
  else
    {
      link_t *cursor = list->first;
      for (int i = 0; cursor != NULL && i < index-1; ++i)
	{
	  cursor = cursor->next;
	}
  
      link_t *tmp = cursor->next;
      link_t *new_link = link_new(elem, tmp);
      cursor->next = new_link;
    }
}



/// Gets the length of a list
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
/// \param delete boolean determining if the function is to
///         use the clean function, true is yes, false if no
void list_remove(list_t *list, int index, bool delete)
{
  // Link to remove
  link_t *remove = list->first;

  int size = list_length(list);
  int pos = index < 0 ? index + size - 1 : index;

  if (size == 0 || pos < 0 || pos > size - 1)
    {
      // Out of bounds
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

      if (pos == size - 1)
	{
	  // Remove last item
	  list->last = prev;
	}
    }
  if (delete)
    {
      (*list->clean_f)(remove->value);
    }
  
  free(remove);
}



/// Iterates through a list and frees all the allocated memory bound to it
///
/// \param list the list
/// \param delete boolean determining if the function is to use the clean
///        function, true is yes, false if no
void list_clear(list_t *list, bool delete)
{
  link_t *cursor = list->first;
  while (cursor != NULL)
    {
      link_t *tmp = cursor;
      cursor = cursor->next;
      if (delete)
        {
          (*list->clean_f)(tmp->value);
        }
      free(tmp);
    }
  
  list->first = NULL;
  list->last = NULL;
}

/// Iterates through a list and frees all the allocated memory bound to it, including itself
///
/// \param list the list
/// \param delete boolean sent to be used in the help function, list_clear
void list_delete(list_t *list, bool delete)
{
  if (list != NULL)
    {
      list_clear(list, delete);
      free(list);
    }
}


/// Get the value of the first element in a list
///
/// \param list the list
/// \param result the element of which the result is designated to return as
bool list_first(list_t *list, elem_t *result)
{
  return list_get(list, 0, result);
}



/// Get the value of the last element in a list
///
/// \param list the list
/// \param result
/// \param result the element of which the result is designated to return as
bool list_last(list_t *list, elem_t *result)
{
  return list_get(list, -1, result);
}


/// Applies a given function to all elements of a list
///
/// \param list the list
/// \param function_apply the function to be applied to all elements
/// \param data data to be sent for further usage in the applied function
/// \returns: boolean returning true if succesful, else false
bool list_apply(list_t *list, elem_apply_fun fun, void *data)
{
  link_t *cursor = list->first;
  while (cursor)
    {
      fun(list->first->value, data);
      cursor = cursor->next;
    }
  
  return true;
}


/// Checks to see if the list contains a specific element and if so returns its index
///
/// \param list the list
/// \param element element to be searched for
/// \returns: integer of which index of the list the value (if it exists) is located at
int list_contains(list_t *list, elem_t elem)
{
  int i = 0;
  link_t *cursor = list->first;
  while (((*list->cmp_f)(cursor->value, elem)) != 0 && cursor != NULL)
  {
    cursor = cursor->next;
    i++;
  }
  return i;
}
