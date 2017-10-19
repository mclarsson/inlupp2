/// !! OBS -- du skall inte ändra i denna fil, med ETT undantag:
/// !! du skall ändra i typedef av L så att det passar ditt program.


#ifndef __list_h__
#define __list_h__

#include <stdbool.h>
#include <stdlib.h>

#include "common.h"

/// This function is used in list_delete() to allow the lists which are
/// the sole owners of their data to free the data on delete.
typedef void(*list_action)(elem_t elem);

/// Define struct list in your .c file not here! (why?)
typedef struct list list_t;

/// \file list.h
///
/// \author Tobias Wrigstad
/// \version 1.2
/// \date 2017-09-13
/// \bug None known. 
///
/// Changelog 2017-09-18
/// Review changes (docs only)
/// Added new helper functions


/// Creates a new list
///
/// \param cmp_f compare function
/// \returns: empty list
list_t *list_new(element_copy_fun copy_fun, element_free_fun clean_fun, element_comp_fun comp_fun);



/// Inserts a new element at the end of the list
///
/// \param list pointer to the list
/// \param elem the element to be appended
void list_append(list_t *list, elem_t elem);

/// Inserts a new element at the beginning of the list
///
/// \param list pointer to the list
/// \param elem the element to be prepended
void list_prepend(list_t *list, elem_t elem);

/// Inserts a new element at a given index. 
///
/// Example:
///
/// list_t *l = list_new(); // l == []
/// list_insert(l, 0, e1);  // l == [e1]
/// list_insert(l, 0, e2);  // l == [e2, e1]
/// list_insert(l, 1, e3);  // l == [e2, e3, e1]
/// list_insert(l, 5, e4);  // l == [e2, e3, e1]
///
/// The last case fails (and returns false) because
/// size is 3, meaning 5 is not a valid index. 
///
/// Note that insert at index 0 is the same as prepend 
/// and insert index size is the same as append. 
///
/// Negative indexes should be supported:
///
/// list_insert(l, -1, e4);       // l == [e2, e3, e1, e4]
///
/// A positive index can be calculated from a 
/// negative like this: pos_i = size + 1 + neg_i.
/// 
/// \param list  pointer to the list
/// \param index the index for elem to be inserted at
/// \param elem  the element to be inserted
/// \returns true if succeeded, else false
void list_insert(list_t *list, int index, elem_t elem);

/// Removes an element from a list.
///
/// Example: (assume l == [e2, e3, e1, e4])
///
/// L elem;
/// list_remove(l, 1, &elem);  // l = [e2, e1, e4], elem == e3
/// list_remove(l, -1, &elem); // l = [e2, e1], elem == e4
///
/// \param list  pointer to the list
/// \param index the index to be removed
/// \param elem a pointer to where the element can be stored
/// \returns true if succeeded, else false
void list_remove(list_t *list, int index, bool delete);

/// Returns the element at a given index
/// \param list  pointer to the list
/// \param index the index to be returned
/// \returns a pointer to the element at index index
bool list_get(list_t *list, int index, elem_t *result);

/// A convenience for list_get(list, 0)
bool list_first(list_t *list, elem_t *result);

/// A convenience for list_get(list, -1)
bool list_last(list_t *list, elem_t *result);

/// Returns the length of the list. It is undefined
/// whether the length is calculated in O(n) time or
/// whether there is a size counter in the list object
/// that is manipulated by insert, remove, etc. 
/// \param list the list
/// \returns the length of list
int list_length(list_t *list);

//////////// ================= Added in version 1.2 
///
/// NOTE: Implementing these functions is NOT mandatory
///


/// Iterates through a list and frees all the allocated memory bound to it
///
/// \param list the list
/// \param delete if true uses own clean function
/// \param cleanup variable to clear all parts of a link
void list_clear(list_t *list, bool delete);

/// Deletes a list. 
///
/// \param list pointer to the list
/// \param cleanup a function that takes an element as
///        argument, to be used to free memory. If this param is 
///        NULL, no cleanup of keys or elements will happen.
void list_delete(list_t *list, bool delete);

/// This function is used in list_apply() to allow applying a function
/// to all elements in a list
typedef void(*list_action2)(elem_t elem, void *data);

/// Applies a function to all elements in a list in list order
///
/// \param list the list
/// \param fun the function to apply to all elements
/// \param data an extra argument passed to each call to fun (may be NULL)
bool list_apply(list_t *list, list_action2 fun, void *data);


int list_contains(list_t *list, elem_t elem);


#endif
