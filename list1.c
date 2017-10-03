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

list_t *list_new()
{
  return calloc(1, sizeof(list_t));
}

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
void list_prepend(list_t *list, L elem)
{
  list->first = link_new(elem, list->first);
  if (list->last == NULL)
    {
      list->last = list->first;
    }
}




L *list_get(list_t *list, int index)
{
  link_t *cursor = list->first;
  for (int i = 0; cursor != NULL && i < index; ++i)
    {
      cursor = cursor->next;
    }
  return cursor->value;
}




bool list_insert(list_t *list, int index, L elem)
{
  if (index == 0)
    {
      list_prepend(list, elem);
      return true;
    }
  else if (index == list_length(list))
    {
      list_append(list, elem);
      return true;
    }
  else if (abs(index) > list_length(list))
    {
      puts("Not a valid index.\n");
      return false;
    }
  else if (index < 0)
    {
      index = list_length(list)  + index;
    }
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
  //new_link->next = tmp;
  
  return true;

}




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
/*
bool shelf_exist(char *shelf, list_t *newlist)
{
  for (int i = 0; i < list_length(newlist); i++)
    {
      if ( newlist[i] == shelf)
      //if (strcmp( (char) (newlist[i], shelf) == 0)
        {
          return true;
        }
    }
  return false;
 }
*/

bool list_remove(list_t *list, int index, L *elem)
{
  if (index == 0)
   {
     link_t *old_first = list->first;
     list->first->next = old_first;
     link_t *remove = NULL;
     list->first = remove;
   }
  link_t *cursor = list->first;
  for (int i = 0; cursor != NULL && i < index - 1; ++i)
    {
      cursor = cursor->next;
    }
  link_t *tmp = cursor->next->next;
  link_t *remove = NULL;
  cursor->next = remove;
  cursor->next = tmp;
  
  
  return true;
}

/*
int main()
{
  list_t *newlist = list_new();

  list_append(newlist, "yxa");
  list_append(newlist, "pinne");
  list_append(newlist, "sten");
  list_append(newlist, "öl");
  list_append(newlist, "korv");
  list_append(newlist, "kniv");
  char *sad = (char *)list_get(newlist, 2);

  printf("%s\n", sad);
  free(newlist);
  
}
*/

