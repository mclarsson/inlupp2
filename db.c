
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <assert.h>

#include "db.h"
#include "utils.h"
#include "list.h"
#include "tree.h"
#include "storage.c"


struct item {
  char *description;
  int price;
  list_t *shelves;
};

struct shelf {
  char *name;
  int amount;
};

struct goods
{
  char *name;
  item_t *item;
};

struct action
{
  enum { NEW, NOTHING, ADD, REMOVE, EDIT } type;
  struct { goods_t saved; shelf_t *saved_shelf; };      // REMOVE
  struct { item_t *edited; item_t original; }; // EDIT
};


/// Frees all the available shelfs in the element
///
/// \param value element to be freed unless == NULL
void free_shelf(elem_t value)
{
  shelf_t *shelf = value.p;
  if (shelf)
    {
      free(shelf->name);
      free(shelf);
    }
}


/// Frees the key-element in a tree-key
///
/// \param key key-value to be freed unless == NULL
void free_key(tree_key_t key)
{
  if (key.p)
    {
      free(key.p);
    }
}

/// Removes item along with all shelves
///
/// \param value item to be freed of all allocated memory
void free_goods(elem_t value)
{
  item_t *item = value.p;
  list_delete(item->shelves, true);
  free(item->description);
  free(item);
}

/// Compare function for shelves
///
/// \param e1 first shelf
/// \param e2 second shelf
/// \returns: like strcmp
int cmp_shelf_names(elem_t e1, elem_t e2)
{
  char *a = (char *) e1.p;
  char *b = (char *) e2.p;
  return strcmp(a, b);
}

/// Creates new action tracker
///
/// \returns: new action
action_t *action_new()
{
  action_t *new = calloc(1, sizeof(action_t));
  new->type = NOTHING;
  
  element_free_fun remove = free_shelf;
  element_comp_fun compare = cmp_shelf_names;
  
  new->original.shelves = list_new(NULL, remove, compare);
  
  return new;
}

/// Frees action_t from memory
///
/// \param action action to remove
void free_action(action_t *action)
{
  list_delete(action->original.shelves, true);

  if (action->original.description != NULL)
    {
      free(action->original.description);
    }
  
  if (action->saved_shelf)
    {
      free(action->saved_shelf->name);
      free(action->saved_shelf);
    }
  
  free(action);
}

/// Creates new shelf
///
/// \param name Name of shelf (shelf format expected)
/// \param amount Amount of items on shelf
/// \returns: pointer to shelf
shelf_t *make_shelf(char *name, int amount)
{
  shelf_t *new = calloc(1, sizeof(shelf_t));
  new->name = name;
  new->amount = amount;
  return new;
}

/// Creates item
///
/// \param description Description of item.
/// \param price Price of item.
/// \returns: pointer to item.
item_t *make_item(char *description, int price)
{
  item_t *new = calloc(1, sizeof(item_t));
  new->description = description;
  new->price = price;
  element_free_fun remove = free_shelf;
  element_comp_fun compare = cmp_shelf_names;
  
  new->shelves = list_new(NULL, remove, compare);
  return new;
}

/// Makes a new goods
///
/// \param name name-key to be used by the item
/// \param item item to be used as bulk of the goods
/// \param shelf new shelf of the item
/// \returns: new goods consisting of the name and item 
goods_t *make_goods(char* name, item_t *item, elem_t shelf)
{
  goods_t *tmp = calloc(1, sizeof(goods_t));
  item->shelves = shelf.p;
  tmp->name = name;
  tmp->item = item;
  return tmp;
}


/// Adds shelf to item
///
/// \param item Item to add to
/// \param name Name of shelf
/// \param amount Amount of item on shelf
void add_shelf(item_t *item, char *name, int amount)
{
  elem_t new = { .p = make_shelf(name, amount) };
  list_append(item->shelves, new);
}


/// Undos latest action, for the moment only edits
///
/// \param tree Pointer to tree in which the action is to be undone
/// \param action Action struct keeping track of what happened previously
void undo_action(tree_t *tree, action_t *action)
{
  if (action->type == EDIT)
    {
      // Copy all old attributes
      free(action->edited->description);
      action->edited->description = strdup(action->original.description);
      action->edited->price = action->original.price;

      // Remove all current shelves
      list_clear(action->edited->shelves, true);
      
      int shelves_length = list_length(action->original.shelves);

      // Add saved shelves
      for (int i = 0; i < shelves_length; ++i)
	{
          elem_t tmp = { .p = NULL };
          list_get(action->original.shelves, i, &tmp);
          shelf_t *tmp_shelf = tmp.p;
          elem_t shelf = { .p = make_shelf(strdup(tmp_shelf->name), tmp_shelf->amount) };
	  list_append(action->edited->shelves, shelf); 
	}
      
      // Remove all saved shelves
      list_clear(action->original.shelves, true);
    }
  else if (action->type == REMOVE)
    {
      tree_key_t key = { .p = action->saved.name };
      if (tree_has_key(tree, key))
	{
	  // Only a shelf was removed
	  shelf_t *tmp = action->saved_shelf;
	  elem_t shelf = { .p = make_shelf(strdup(tmp->name), tmp->amount) };
	  list_append(action->saved.item->shelves, shelf);
	}
      else
	{
	  // Entire item was removed
	  elem_t item = { .p = action->saved.item };
	  tree_insert(tree, key, item);
	}
    }
  else if (action->type == ADD)
    {
      puts("Kan inte ångra lägga till");
    }
  else if (action->type == NOTHING)
    {
      puts("Finns inget att ångra!");
    }
  
  // Förhindrar multipla ångranden
  action->type = NOTHING;
}

/// Prints name and item
///
/// \param name name of ware
/// \param item item which details are printed out
void print_item(char *name, item_t *item)
{
  output("Namn", name);
  output("Beskr.", item->description);
  output_price("Pris", (item->price));

  int shelves_length = list_length(item->shelves);
  for (int i = 0; i < shelves_length; ++i)
    {
      puts("");
      elem_t tmp = { .p = NULL };
      list_get(item->shelves, i, &tmp);
      shelf_t *tmp_shelf = tmp.p;
      output("Hylla", tmp_shelf->name);
      output_int("Antal", (tmp_shelf->amount));

    }
}

/// Searches for and gets a shelf
///
/// \param item the item to be searched
/// \param shelf_name shelf name that is searched for
/// \returns: specific shelf that was searched for
shelf_t *get_item_shelf(item_t *item, char *shelf_name)
{
  list_t *shelves = item->shelves;
  int shelf_length = list_length(shelves);
      
  for (int j = 0; j < shelf_length; ++j)
    {
      elem_t tmp = { .p = NULL };
      list_get(item->shelves, j, &tmp);
      shelf_t *tmp_shelf = tmp.p;
      
      if (strcmp(shelf_name, tmp_shelf->name) == 0)
	{
	  return tmp_shelf;
	}
    }

  return NULL;
}


/// Checks to see if an item contains a shelf
///
/// \param item item to be seached
/// \param shelf_name shelf name that is seached for
/// \returns: true if shelf was found, else false
bool item_has_shelf(item_t *item, char *shelf_name)
{
  return get_item_shelf(item, shelf_name) != NULL;
}



/// Check if shelf exists in system
///
/// \param tree the tree to be searched
/// \param name shelf name to be searched for
/// \returns: true if shelf is already in use, else false
bool shelf_exists(tree_t *tree, char *name)
{
  elem_t *items = tree_elements(tree);
  int size = tree_size(tree);
  bool exists = false;
  
  for (int i = 0; i < size; ++i)
    {
      exists = item_has_shelf(items[i].p, name);
      if (exists)
	{
	  break;
	}
    }

  free(items);
  return exists;
}


/// Asks user for a shelf name until an unique one is entered
///
/// \param tree tree to check uniqueness in
/// \returns: unique shelf name
char *unique_shelf(tree_t *tree)
{
  bool exists = true;

  do {
    char *name = ask_question_shelf("Hylla:");
    bool exists = shelf_exists(tree, name);

    if (exists)
      {
	puts("Hyllan finns redan, välj en annan");
      }
    else
      {
	return name;
      }
    
  } while (exists);

  return NULL;
}

/// Asks for input and creates shelf
///
/// \param tree the tree where the shelf is created in
/// \returns: new shelf
shelf_t *input_shelf(tree_t *tree)
{
  char *name = unique_shelf(tree);
  int amount = ask_question_int("Antal:");
  return make_shelf(name, amount);
}

/// Asks for inputs and creates item
///
/// \param tree the tree in which the item is created
/// \returns: new item
item_t *input_item(tree_t *tree)
{
  char *description = ask_question_string("Beskrivning:");
  int price = ask_question_int("Pris:");
  
  item_t *item = make_item(description, price);
  elem_t shelf = { .p = input_shelf(tree) };
  list_append(item->shelves, shelf);
  
  return item;
}


/// Helps insert an item into a tree
///
/// \param tree the tree
/// \param key tree-key to be used for name of the item
/// \param item the item
void insert_goods(tree_t *tree, tree_key_t key, elem_t item)
{
  tree_insert(tree, key, item);
}


/// Checks if the shelf is in use and if not
/// inserts the elements of a shelf into an item into a tree
///
/// \param tree the tree
/// \param item the item
/// \param shelf_name name of the shelf
/// \param amount amount of the shelf
/// \returns: true if successful, else false
bool insert_shelf(tree_t *tree, item_t *item, char  *shelf_name, int amount)
{
  if (!shelf_exists(tree, shelf_name))
    { 
      // Add new shelf
      add_shelf(item, shelf_name, amount);
      return true;
    }
  else
    {
      shelf_t *match = get_item_shelf(item, shelf_name);
      if (match != NULL)
	{ 
	  // Aggregate shelf amount
	  match->amount += amount;
	  return true;
	}
    }

  return false;
}

/// Adds item to tree
///
/// \param tree the tree in which the item is added
/// \param action what to do in case of an undo call
void add_goods(tree_t *tree, action_t *action)
{ 
  bool abort = false;
  bool save = false;

  action->type = ADD;

  // Get inputs until aborted or saved
  while (!(abort || save))
    {
      tree_key_t key = { .p = ask_question_string("Namn:") };
    
      if (tree_has_key(tree, key))
	{
	  puts("Varan finns redan, lägger till ny hylla");
	
	  elem_t elem = { .p = NULL };
	  tree_get(tree, key, &elem);
	  item_t *item = elem.p;
	  free(key.p);

	  // Ask for shelfname until it's either new or if it already belongs to item
	  while (!save)
	    {
	      char *shelf_name = ask_question_shelf("Hylla:");

	      if (shelf_exists(tree, shelf_name) && !item_has_shelf(item, shelf_name))
		{
		  puts("Hyllan tillhör inte varan");
		}
	      else
		{
		  int amount = ask_question_int("Antal:");
		  save = insert_shelf(tree, item, shelf_name, amount);
		}

	      free(shelf_name);
	    }
	}
      else
	{
	  // Name not already in tree

	  // Create item from inputs
	  elem_t item = { .p = input_item(tree) };

	  // Confirm item
	  print_item(key.p, item.p);
	
	  puts("\nVill du lägga till?");
	  char input = ask_question_char_in_str("[J]a, [n]ej, [r]edigera", "JNR");

	  if (input == 'J')
	    {
	      insert_goods(tree, key, item);
	      save = true;
	    }
	  else if (input == 'N')
	    {
	      // Free what is not to be saved
              //elem_t null = { .p = NULL };
	      list_remove(((item_t *) item.p)->shelves, 0, true);
	      free(((item_t *) item.p)->shelves);
	      free(item.p);
	    
	      abort = true;
	    }
	}
    }
}




/// Presents list of items and returns chosen item along with name
///
/// \param tree tree to display
/// \returns: chosen item
goods_t select_goods(tree_t *tree)
{
  int size = tree_size(tree);
  tree_key_t *items = tree_keys(tree);
  int page_size = 20;
  
  // Current index for iteration through items
  int index = 0;
  int current_page = 1;
  bool view_next = true;
  
  while (size > index && view_next)
    {
      printf("\nSida: %d \n\n", current_page);
      
      int max;
      if (current_page * page_size > size) max = size - index;
      else max = page_size;

      // Print page
      for (int k = 1; k <= max; ++k, ++index)
	{
	  printf("%d.\t%s\n", k, (char *)items[index].p);
	}

      char *input = ask_question_string("\nVälj vara [nummer], [n]ästa sida eller [a]vbryt");
	
      if (is_number(input))
	{
	  // User selected a goods
	  
	  int input_index = atoi(input);

	  if (input_index > 0 && input_index <= max)
	    {
	      int item_index  = (input_index - 1) + (current_page - 1) * page_size;
	      elem_t elem = { .p = NULL };
	      tree_get(tree, items[item_index], &elem);
	      item_t *item = elem.p;
	      char *name = items[item_index].p;
	  
	      free(input);
	      free(items);
	      return (goods_t) { .name = name, .item = item };
	    }
	}
      else if (strcmp(input, "a") == 0)
	{
	  view_next = false;
	}
      else if(strcmp(input, "n") == 0)
	{
	  ++current_page;
	}
      
      free(input);
    }
  

  free(items);
  return (goods_t) { .name = NULL, .item = NULL };
}


/// Removes a shelf from an item
///
/// \param item the item
/// \param index the index of the desired shelf
/// \param action replaces the previous action with a new one
void remove_shelf(item_t *item, int index, action_t *action)
{
  int shelves_length = list_length(item->shelves);
  if (index >= 0 && index < shelves_length)
    {
      // save removed shelf
      elem_t tmp = { .p = NULL };
      list_get(item->shelves, index, &tmp);
      shelf_t *tmp_shelf = tmp.p;
      action->saved_shelf = make_shelf(strdup(tmp_shelf->name), tmp_shelf->amount);
      	  
      //elem_t val = { .p = NULL };
      list_remove(item->shelves, index, true);
      
      action->type = REMOVE;
    }
}


/// Removes a goods from tree catalog
///
/// \param tree the tree
/// \param goods the goods
/// \param action replaces the previous action with a new one
void remove_from_catalog(tree_t *tree, goods_t goods, action_t *action)
{
  elem_t *elem = NULL;
  tree_remove(tree, (tree_key_t)  { .p = goods.name }, elem); 
  action->type = REMOVE;
}


/// Removes all parts of a goods from a tree
///
/// \param tree the tree
/// \param action replaces the previous action with a new one
void remove_goods(tree_t *tree, action_t *action)
{
  goods_t selected = select_goods(tree);

  if (selected.name == NULL) return;
  
  int shelf_length = list_length(selected.item->shelves);
  action->saved = selected;

  // If item has more than one shelf, individual shelf is removed
  if (shelf_length > 1)
    {
      // List shelves
      for (int i = 1; i <= shelf_length; ++i)
	{
          elem_t tmp = { .p = NULL };
          list_get(selected.item->shelves, i-1, &tmp);
          shelf_t *tmp_shelf = tmp.p;
	  printf("\n%d. %s (%d stycken)", i, tmp_shelf->name, tmp_shelf->amount);
	}

      int index = ask_question_int("\n\nVilken plats skall tas bort (0 för ingen)?") - 1;

      remove_shelf(selected.item, index, action);
    }
  else
    {
      remove_from_catalog(tree, selected, action);
    }
}

/// Presents list of items in tree
///
/// \param tree tree to be displayed
void list_goods(tree_t *tree)
{
  bool stop = false;
  do {
    goods_t selected = select_goods(tree);
    
    if (selected.name != NULL && selected.item != NULL)
      {
	print_item(selected.name, selected.item);
      }
    else
      {
        puts("Finns inget att visa!");
	stop = true;
      }
  } while (!stop);
}

/// Edits parameters of current items
///
/// \param tree the tree containing the items
/// \param action what to do in case of an undo call
void edit_goods(tree_t *tree, action_t *action)
{ 
  goods_t goods = select_goods(tree);

  if (goods.name == NULL && goods.item == NULL)
    {
      puts("Finns inget att redigera!");
      return;
    }
  
  list_t *shelves = goods.item->shelves;

  // Copy item for undo
  action->type = EDIT;
  action->edited = goods.item;
  free(action->original.description);
  action->original.description = strdup(goods.item->description);
  action->original.price = goods.item->price;

  // Clear copys shelves
  list_clear(action->original.shelves, true); 
  
  // Copy shelves
  int shelves_length = list_length(shelves);
  for (int i = 0; i < shelves_length; i++)
    {
      elem_t tmp = { .p = NULL };
      list_get(shelves, i, &tmp);
      shelf_t *tmp_shelf = tmp.p;
      elem_t tmpshelf = { .p = make_shelf(strdup(tmp_shelf->name), tmp_shelf->amount) };
      
      list_append(action->original.shelves, tmpshelf); 
    }
  // Prints the selected item for the user
  print_item(goods.name, goods.item);

  puts("\n[B]eskrivning");
  puts("[P]ris");
  puts("[L]agerhylla");
  puts("An[t]al\n");

  char input = ask_question_char_in_str("Välj rad eller [a]vbryt", "BPLTA");

  switch (input)
    {

      // Edit the description
    case 'B':
      output("Nuvarande beskrivning", goods.item->description);
      puts("----------------------");
      free(goods.item->description);
      goods.item->description = ask_question_string("Ny beskrivning: ");
      break;


      // Edit the price
    case 'P':
      output_price("Nuvarande pris", goods.item->price);
      puts("---------------");
      goods.item->price = ask_question_int("Nytt pris: ");
      break;

      
      // Edit the shelf-name
    case 'L':
      puts("Nuvarande hyllor");
      puts("----------------");
      
      for (int i = 0; i < shelves_length; ++i)
	{
          elem_t tmp = { .p = NULL };
          list_get(shelves, i, &tmp);
          shelf_t *tmp_shelf = tmp.p;
          printf("%d. Namn:  %s\n", i+1, tmp_shelf->name);
          printf("   Mängd: %d\n\n", tmp_shelf->amount);

        }
      int index_shelf = ask_question_int("Vilken hylla vill du ändra?");
      elem_t tmp = { .p = NULL };
      list_get(shelves, index_shelf, &tmp);
      shelf_t *tmp_shelf = tmp.p;
      tmp_shelf->name = ask_question_shelf("Ny hylla: ");
      break;
      

      // Edit the amount  
    case 'T':
      for (int i = 0; i < shelves_length; ++i)
	{
          elem_t tmp = { .p = NULL };
          list_get(shelves, i, &tmp);
          shelf_t *tmp_shelf = tmp.p;
          printf("%d. Namn:  %s\n", i+1, tmp_shelf->name);
          printf("   Mängd: %d\n\n", tmp_shelf->amount);
          
        }
      int index_amount = ask_question_int("Vilket antal vill du ändra?");

      elem_t tmp_value = { .p = NULL };
      list_get(shelves, index_amount, &tmp_value);
      shelf_t *tmp_amount = tmp.p;
      tmp_amount->amount = ask_question_int("Nytt antal: ");
      break;
      
    case 'A':
      return;
    default:
      break;
    }
}

/// Store item in file along with shelves
///
/// \param name name of item
/// \param value tree value with item
/// \param data pointer to file
bool store_item(tree_key_t name, elem_t value, void *data)
{
  FILE *file = data;
  item_t *item = value.p;
  int shelves_length = list_length(item->shelves);
  
  // Store name
  store_int(file, strlen(name.p) + 1);
  store_string(file, name.p);

  // Store description
  store_int(file, strlen(item->description) + 1);
  store_string(file, item->description);

  // Store price
  store_int(file, item->price);

  // Number of shelves
  store_int(file, shelves_length);

  // Store all shelves
  for (int i = 0; i < shelves_length; ++i)
    {
     
      elem_t tmp = { .p = NULL };
      list_get(item->shelves, i, &tmp);
      shelf_t *tmp_shelf = tmp.p;

      // Shelf name
      store_int(file, strlen(tmp_shelf->name) + 1);
      store_string(file, tmp_shelf->name);

      // Amount
      store_int(file, tmp_shelf->amount);
    }

  return true;
}

/// Save entire catalog to file
///
/// \param catalog tree with items
/// \param save_file file to save catalog to
void save_catalog(tree_t *catalog, FILE *save_file)
{ 
  int size = tree_size(catalog);

  fseek(save_file, 0, SEEK_SET);

  store_int(save_file, size);
  tree_apply(catalog, preorder, &store_item, (void *) save_file);
}

/// Loads content of file into catalog. If no such file exists it is created.
///
/// \param catalog tree to insert into
/// \param name name of save file
/// \returns: pointer to save_file
FILE *load_catalog(tree_t *catalog, char *name)
{
  FILE *file = open_file(name);

  fseek(file, 0, SEEK_SET);
  
  int items = load_int(file);

  for (int i = 0; i < items; ++i)
    {
      // Load name
      int name_size = load_int(file);
      char *name = load_string(file, name_size);

      // Load description
      int desc_size = load_int(file);
      char *desc = load_string(file, desc_size);

      // Load price
      int price = load_int(file);
      
      item_t *item = make_item(desc, price);

      // Load shelves
      int shelves_length = load_int(file);
      for (int s = 0; s < shelves_length; ++s)
	{
	  // Shelf name
	  int s_name_size = load_int(file);
	  char *s_name = load_string(file, s_name_size);

	  // Shelf amount
	  int amount = load_int(file);
	  
	  add_shelf(item, s_name, amount);
	}

      tree_key_t key = { .p = name };
      elem_t value = { .p = item };
      tree_insert(catalog, key, value);
    }

  return file;
}
