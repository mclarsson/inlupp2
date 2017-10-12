#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "list.h"


/// Struct for list
struct list {
  link_t *first;
  link_t *last;
  cmp_t *cmp_f;
};


union element
{
  void *p;
  int   i;
  uint  u;
  float f;
};


/// Struct for link-content
struct link
{
  element_t value;
  link_t *next;
  //cmp_t *cmp_f;
};


/// Allocates memory for/and creates a list
///
/// \returns: empty list
list_t *list_new(cmp_t *cmp)
{
  list_t *new = calloc(1, sizeof(list_t));
  if (new)
  {
    new->cmp_f = cmp;
  }
  return new;
}


/// Creates a link between current value and next link
///
/// \param value of new element
/// \param next next link in line
/// \returns: link, part of a list
link_t *link_new(element_t value, link_t *next)
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
void list_append(list_t *list, element_t elem)
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
void list_prepend(list_t *list, element_t elem)
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
element_t list_get(list_t *list, int index)
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
/*bool list_insert(list_t *list, int index,  element_t *elem)
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
*/

void list_insert(list_t *l, element_t elem)
{
  link_t **c = &(l->first);
  while (*c && (*l->cmp_f)((*c)->value, elem))
    {
      c = &((*c)->next);
    }
  *c = link_new(elem, *c);
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
bool list_remove(list_t *list, int index, element_t elem)
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
  
  if (elem.p != NULL) elem = remove->value;
  
  free(remove);
  return true;
}

/// Iterates through a list and frees all the allocated memory bound to it
///
/// \param list the list
/// \param cleanup variable to clear all parts of a link
void list_delete(list_t *list, list_action cleanup)
{
  link_t *cursor = list->first;
  while (cursor !=NULL)
    {
      link_t *tmp = cursor;
      cursor = tmp->next;
      cleanup(tmp->value);
      free(tmp);
    }
  
  free(list);
}


/// Get the value of the first element in a list
///
/// \param list the list
/// \returns: pointer to the first value in list
element_t list_first(list_t *list)
{
  return list->first->value;
}



/// Get the value of the last element in a list
///
/// \param list the list
/// \returns: pointer to the last value in list
element_t list_last(list_t *list)
{
  return list->last->value;
}

void cleanup(element_t elem)
{
  elem.i = 0;
}

/*
int cmp_ex(element_t e1, element_t e2)
{
  return e1.i < e2.i ? -1 : e1.i == e2.i ? 0 : 1;
}



int main(int argc, char *argv[])
{
  cmp_t cmp = &cmp_ex;
  list_t *new = list_new(&cmp);
  //int length = 100;
  for (int index = 0; index < length; ++index)
    {
      //printf("Link: %d\n", i);
      //int a = index;
      // element_t e;
      //e.i = a;
      
      list_insert(new, &((element_t) { .i = index }));
    }
  
  list_insert(new, ((element_t) { .i = 1 }));
  list_insert(new, ((element_t) { .i = 2 }));
  list_insert(new, ((element_t) { .i = 3 }));
  list_insert(new, ((element_t) { .i = 4 }));
  
  for (int index = 0; index < 4; ++index)
    {
      element_t asd = list_get(new, index);
      //int asd2 = asd.i;
      printf("List_get link: %d\n", asd.i);
    }
  
  
  list_delete(new, cleanup);

  return 0;
}
*/
