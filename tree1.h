#ifndef __tree_h__
#define __tree_h__

#include <stdbool.h>
#include "list.h"

/// Change this definition and replace int with the appropriate type
/// in your program. Note, however, that your implementation of tree.c
/// should treat T as if it did not know the type.

typedef void *T;

//typedef struct item T;

/// For now, let an element's key be string 
typedef char *K;

/// Define struct tree in your .c file not here! (why?)
typedef struct tree tree_t;

/// This function is used in tree_delete() to allow the trees which are
/// the sole owners of their data to free the data on delete.
//typedef void(*tree_action)(K key, T elem);

/// \file tree.h
///
/// \author Tobias Wrigstad
/// \version 1.2
/// \date 2017-09-18
///
/// Changelog 2017-09-13
/// Updated the documentation
///
/// Changelog 2017-09-18
/// Review changes (docs only)
/// Fixed macro bug in tree_update
/// Added new helper functions


/// Creates a new tree
///
/// \returns: empty tree
tree_t *tree_new();

/// Remove a tree along with all T elements.
///
/// \param tree the tree
/// \param cleanup a function that takes a key and element as
///        argument, to be used to free memory. If this param is 
///        NULL, no cleanup of keys or elements will happen.
void tree_delete(tree_t *tree);

/// Get the size of the tree 
///
/// \returns: the number of nodes in the tree
int tree_size(tree_t *tree);

/// Get the depth of the tree 
///
/// \returns: the depth of the deepest subtree
int tree_depth(tree_t *tree);

/// Insert element into the tree. Returns false if the key is already used.
///
/// \param tree pointer to the tree
/// \param key the key of element to be appended
/// \param elem the element 
/// \returns: true if successful, else false
bool tree_insert(tree_t **tree, K key, T elem);

/// Checks whether a key is used in a tree
///
/// \param tree pointer to the tree
/// \param key the key of elem to be removed
/// \returns: true if key is a key in tree
bool tree_has_key(tree_t *tree, K key);

/// Returns the element for a given key in tree.
/// (The implementation may assume that the key exists.)
/// 
/// \param tree pointer to the tree
/// \param key the key of elem to be removed
/// \returns: true if key is a key in tree
T tree_get(tree_t *tree, K key);

tree_t *tree_maker(tree_t *tree, K key, T elem);

void PrintInorder(tree_t *tree, int *index);

void appendInorder(tree_t *tree, list_t *newlist);
void appendKeyInorder(tree_t *tree, list_t *newlist);
//list_t FlatOrder(tree_t *tree, list_t empty);

  
/// This does not need implementation until Assignment 2
///
/// \param tree pointer to the tree
/// \param key the key of elem to be removed
/// \returns: the removed element
T tree_remove(tree_t *tree, K key);

/// Swap the element for a given key for another.
///
/// \param tree pointer to the tree
/// \param key the key of elem to be changed
/// \param elem the updated element 
/// \returns: the updated element
#define tree_update(t, k, e)                    \
  { T tmp = tree_remove(t, k);                  \
    tree_insert(t, k, e);                       \
    tmp; }                                      \

#endif
