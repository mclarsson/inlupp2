#ifndef __db_h__
#define __db_h__

#include "list.h"
#include "tree.h"

typedef struct shelf shelf_t;

typedef struct item item_t;

typedef struct action action_t;

action_t *action_new();

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

void add_goods(tree_t *tree, action_t *action);

//char *select_goods(tree_t *tree)

void list_goods(tree_t *tree);

// void edit_goods(tree_t *tree)

void edit_goods(tree_t *tree, action_t *action);

void remove_goods(tree_t *tree);

void undo_action(tree_t *tree, action_t *action);

#endif
