#ifndef __db_h__
#define __db_h__

#include <stdio.h>

#include "list.h"
#include "tree.h"

typedef struct action action_t;

action_t *action_new();


/// Frees action_t from memory
///
/// \param action action to remove
void free_action(action_t *action);

/* TODO: har kvar dessa så länge men misstänker att de kan vara privata
/// Creates new shelf
///
/// \param name Name of shelf (shelf format expected)
/// \param amount Amount of items on shelf
/// \returns pointer to shelf
shelf_t *make_shelf(char *name, int amount);

/// Creates item
///
/// \param description Description of item.
/// \param price Price of item.
/// \returns pointer to item.
item_t *make_item(char *description, int price);

/// Adds shelf to item
///
/// \param item Item to add to
/// \param name Name of shelf
/// \param amount Amount of item on shelf
void add_shelf(item_t *item, char *name, int amount);
*/

void free_goods(elem_t value);

void free_key(tree_key_t key);

/// Creates new items
///
/// \param tree tree to add to
/// \param action to keep track of what's added
void add_goods(tree_t *tree, action_t *action);

/// Presents list of items in tree
///
/// \param tree tree to be displayed
void list_goods(tree_t *tree);

void edit_goods(tree_t *tree, action_t *action);

/// Presents user with list of items, removes selected item
///
/// \param tree tree to remove in
/// \param action action to save removed item in
void remove_goods(tree_t *tree, action_t *action);

/// Undos latest action, for the moment only edits
///
/// \param tree Pointer to tree in which the action is to be undone
/// \param action Action struct keeping track of what happened previously
void undo_action(tree_t *tree, action_t *action);

/// Save entire catalog to file
///
/// \param catalog tree with items
/// \param save_file file to save catalog to
void save_catalog(tree_t *catalog, FILE *save_file);

/// Loads content of file into catalog. If no such file exists it is created.
///
/// \param catalog tree to insert into
/// \param name name of save file
/// \returns: pointer to save_file
FILE *load_catalog(tree_t *catalog, char *name);

#endif
