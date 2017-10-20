#ifndef __db_h__
#define __db_h__

#include <stdio.h>

#include "list.h"
#include "tree.h"


// Different kinds of structs
typedef struct shelf shelf_t;

typedef struct item item_t;

typedef struct goods goods_t;

typedef struct action action_t;




/// Frees all the available shelfs in the element
///
/// \param value element to be freed unless == NULL
void free_shelf(elem_t value);



/// Frees the key-element in a tree-key
///
/// \param key key-value to be freed unless == NULL
void free_key(tree_key_t key);



/// Removes item along with all shelves
///
/// \param value item to be freed of all allocated memory
void free_goods(elem_t value);



/// Compare function for shelves
///
/// \param e1 first shelf
/// \param e2 second shelf
/// \returns: like strcmp
int cmp_shelf_names(elem_t e1, elem_t e2);


  
/// Creates new action tracker
///
/// \returns: new action
action_t *action_new();



/// Frees action_t from memory
///
/// \param action action to remove
void free_action(action_t *action);


 
/// Creates new shelf
///
/// \param name Name of shelf (shelf format expected)
/// \param amount Amount of items on shelf
/// \returns: pointer to shelf
shelf_t *make_shelf(char *name, int amount);



/// Creates item
///
/// \param description Description of item.
/// \param price Price of item.
/// \returns: pointer to item.
item_t *make_item(char *description, int price);



/// Makes a new goods
///
/// \param name name-key to be used by the item
/// \param item item to be used as bulk of the goods
/// \param shelf new shelf of the item
/// \returns: new goods consisting of the name and item 
goods_t *make_goods(char* name, item_t *item, elem_t shelf);



/// Adds shelf to item
///
/// \param item Item to add to
/// \param name Name of shelf
/// \param amount Amount of item on shelf
void add_shelf(item_t *item, char *name, int amount);



/// Undos latest action, for the moment only edits
///
/// \param tree Pointer to tree in which the action is to be undone
/// \param action Action struct keeping track of what happened previously
void undo_action(tree_t *tree, action_t *action);



/// Prints name and item
///
/// \param name name of ware
/// \param item item which details are printed out
void print_item(char *name, item_t *item);



/// Searches for and gets a shelf
///
/// \param item the item to be searched
/// \param shelf_name shelf name that is searched for
/// \returns: specific shelf that was searched for
shelf_t *get_item_shelf(item_t *item, char *shelf_name);



/// Checks to see if an item contains a shelf
///
/// \param item item to be seached
/// \param shelf_name shelf name that is seached for
/// \returns: true if shelf was found, else false
bool item_has_shelf(item_t *item, char *shelf_name);



/// Check if shelf exists in system
///
/// \param tree the tree to be searched
/// \param name shelf name to be searched for
/// \returns: true if shelf is already in use, else false
bool shelf_exists(tree_t *tree, char *name);



/// Asks user for a shelf name until an unique one is entered
///
/// \param tree tree to check uniqueness in
/// \returns: unique shelf name
char *unique_shelf(tree_t *tree);



/// Asks for input and creates shelf
///
/// \param tree the tree where the shelf is created in
/// \returns: new shelf
shelf_t *input_shelf(tree_t *tree);



/// Asks for inputs and creates item
///
/// \param tree the tree in which the item is created
/// \returns: new item
item_t *input_item(tree_t *tree);



/// Helps insert an item into a tree
///
/// \param tree the tree
/// \param key tree-key to be used for name of the item
/// \param item the item
void insert_goods(tree_t *tree, tree_key_t key, elem_t item);



/// Checks if the shelf is in use and if not
/// inserts the elements of a shelf into an item into a tree
///
/// \param tree the tree
/// \param item the item
/// \param shelf_name name of the shelf
/// \param amount amount of the shelf
/// \returns: true if successful, else false
bool insert_shelf(tree_t *tree, item_t *item, char  *shelf_name, int amount);



/// Creates new items
///
/// \param tree tree to add to
/// \param action to keep track of what's added
void add_goods(tree_t *tree, action_t *action);



/// Presents list of items and returns chosen item along with name
///
/// \param tree tree to display
/// \returns: chosen item
goods_t select_goods(tree_t *tree);




/// Removes a shelf from an item
///
/// \param goods goods with item
/// \param index the index of the desired shelf
/// \param action replaces the previous action with a new one
void remove_shelf(goods_t goods, int index, action_t *action);



/// Removes a goods from tree catalog
///
/// \param tree the tree
/// \param goods the goods
/// \param action replaces the previous action with a new one
void remove_from_catalog(tree_t *tree, goods_t goods, action_t *action);



/// Presents user with list of items, removes selected item
///
/// \param tree tree to remove in
/// \param action action to save removed item in
void remove_goods(tree_t *tree, action_t *action);



/// Presents list of items in tree
///
/// \param tree tree to be displayed
void list_goods(tree_t *tree);



/// Edits parameters of current items
///
/// \param tree the tree containing the items
/// \param action what to do in case of an undo call
void edit_goods(tree_t *tree, action_t *action);



/// Store item in file along with shelves
///
/// \param name name of item
/// \param value tree value with item
/// \param data pointer to file
bool store_item(tree_key_t name, elem_t value, void *data);



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
