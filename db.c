
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#include "db.h"
#include "utils.h"
#include "list.h"
#include "tree.h"

//
// Types
//

typedef struct shelf shelf_t;

typedef struct item item_t;

typedef struct goods goods_t;

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
  enum { NOTHING, ADD, REMOVE, EDIT } type;
  union
  {
    struct { goods_t saved; };      // REMOVE
    struct { item_t *edited; item_t original; }; // EDIT
  };
};

void free_shelf(list_value_t value)
{
  shelf_t *shelf = value.p;
  free(shelf->name);
  free(shelf);
}

/// Removes item along with all shelves
void free_goods(tree_key_t key, tree_value_t value)
{
  item_t *item = value.p;
  list_delete(item->shelves, &free_shelf);
  free(item->description);
  free(item);
  free(key.p);
}

/// Compare function for shelves
///
/// \param e1 first shelf
/// \param e2 second shelf
/// \returns: like strcmp
int cmp_shelf_names(list_value_t e1, list_value_t e2)
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
  new->original.shelves = list_new((list_cmp_t *) &cmp_shelf_names);
  new->type = NOTHING;
  return new;
}

/// Frees action_t from memory
///
/// \param action action to remove
void free_action(action_t *action)
{
  if (action->type == EDIT)
    {
      list_delete(action->original.shelves, &free_shelf);
      free(action->original.description);
    }
  else if (action->type == ADD || action->type == REMOVE)
    {
      tree_key_t key = { .p = action->saved.name };
      tree_value_t item = { .p = action->saved.item };
      free_goods(key, item);
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
  new->shelves = list_new((list_cmp_t *) &cmp_shelf_names);
  return new;
}

/// Adds shelf to item
///
/// \param item Item to add to
/// \param name Name of shelf
/// \param amount Amount of item on shelf
void add_shelf(item_t *item, char *name, int amount)
{
  list_value_t new = { .p = make_shelf(name, amount) };
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
      list_clear(action->edited->shelves, &free_shelf);
      
      int shelves_length = list_length(action->original.shelves);

      // Add saved shelves
      for (int i = 0; i < shelves_length; ++i)
	{
	  shelf_t *tmp = (shelf_t*) list_get(action->original.shelves, i).p;
          list_value_t shelf = { .p = make_shelf(strdup(tmp->name), tmp->amount) };
	  list_append(action->edited->shelves, shelf); 
	}
      
      // Remove all saved shelves
      list_clear(action->original.shelves, &free_shelf);
    }
  else if (action->type == REMOVE)
    {
      tree_key_t key = { .p = action->saved.name };
      tree_value_t item = { .p = action->saved.item };
      tree_insert(tree, key, item);
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

/// Checks to see if the two keys are the same
///
/// \param s1 string one
/// \param s2 string two
/// \returns: true if they are the same, else false
bool is(char *s1, char *s2)
{
  return strcmp(s1, s2) == 0;
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
      shelf_t *s = (shelf_t *) list_get(item->shelves, i).p;
      output("Hylla", s->name);
      output_int("Antal", (s->amount));

    }
}

/// Check if shelf exists in system
///
/// \param tree the tree to be searched
/// \param name shelf name to be searched for
/// \returns: true if shelf is already in use, else false
bool shelf_exists(tree_t *tree, char *name)
{
  tree_value_t *items = tree_values(tree);
  int size = tree_size(tree);
  
  for (int i = 0; i < size; ++i)
    {
      item_t *item = items[i].p;
      list_t *shelves = item->shelves;
      int shelf_length = list_length(shelves);
      
      for (int j = 0; j < shelf_length; ++j)
	{
          shelf_t *shelf = (shelf_t *) list_get(shelves, j).p;
          

	  if (strcmp(name, shelf->name) == 0)
	    {
	      free(items);
	      return true;
	    }
	}
    }

  free(items);
  return false;
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
  list_value_t shelf = { .p = input_shelf(tree) };
  list_append(item->shelves, shelf);
  
  return item;
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
  do {
    tree_key_t key = { .p = ask_question_string("Namn:") };
    
    if (tree_has_key(tree, key))
      {
	puts("Varan finns redan, lägger till ny hylla");
	
        item_t *item = (item_t*) tree_get(tree, key).p;
	free(key.p);

	// Ask for shelfname until it's either new or if it already belongs to item
	do {
	  char *shelf_name = ask_question_shelf("Hylla:");
	  
	  if (!shelf_exists(tree, shelf_name))
	    { 
	      // Add new shelf
	      int amount = ask_question_int("Antal:");
              list_value_t shelf = { .p = make_shelf(shelf_name, amount) };

	      list_append(item->shelves, shelf);

	      save = true;
	    }
	  else
	    {
	      // Shelf exists, check if item has shelf
	      int shelf_length = list_length(item->shelves);
	      for (int i = 0; i < shelf_length; ++i)
		{

		  shelf_t *match = (shelf_t *) list_get(item->shelves, i).p;
		  if (strcmp(shelf_name, match->name) == 0)
		    {
		      // Aggregate shelf amount
		      match->amount += ask_question_int("Lägg till antal:");
		      save = true;
		      break;
		    }
		}

	      if (!save)
		{
		  puts("Hyllan tillhör inte varan");
		}
	    }
	  
	} while(!save);
      }
    else
      {
	// Name not already in tree

	// Create item from inputs
        tree_value_t item = { .p = input_item(tree) };

	// Confirm item
	print_item(key.p, item.p);
	
	puts("\nVill du lägga till?");
	char input = ask_question_char_in_str("[J]a, [n]ej, [r]edigera", "JNR");

	if (input == 'J')
	  {
	    tree_insert(tree, key, item);
	    save = true;
	  }
	else if (input == 'N')
	  {
	    // Free what is not to be saved
            list_value_t null = { .p = NULL };
	    list_remove(((item_t *) item.p)->shelves, 0, null);
	    free(((item_t *) item.p)->shelves);
	    free(item.p);
	    
	    abort = true;
	  }
      }
  } while (!(abort || save));
}

/// Presents list of items and returns chosen item along with name
///
/// \param tree tree to display
/// \returns chosen item
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

      char *input = ask_menu_option("\nVälj vara [1-20], [n]ästa sida eller [a]vbryt");
	
      if (is_number(input))
	{
	  // User selected a goods
	  
	  int input_index = atoi(input);
	    
	  int item_index  = (input_index - 1) + (current_page - 1) * page_size;
	  item_t *item = tree_get(tree, items[item_index]).p;
	  char *name = items[item_index].p;
	  
	  free(input);
	  free(items);
	  return (goods_t) { .name = name, .item = item };
	}
      else if (is(input, "A"))
	{
	  free(input);
	  view_next = false;
	}
      else if(is(input, "N"))
	{
	  ++current_page;
	}
    }
  

  free(items);
  return (goods_t) { .name = NULL, .item = NULL };
}

void remove_goods(tree_t *tree, action_t *action)
{
  goods_t selected = select_goods(tree);
  int shelf_length = list_length(selected.item->shelves);
  
  if (shelf_length > 1)
    {
      for (int i = 1; i <= shelf_length; ++i)
	{
	  shelf_t *shelf = list_get(selected.item->shelves, i - 1).p;
	  printf("\n%d. %s (%d stycken)", i, shelf->name, shelf->amount);
	}

      int index = ask_question_int("\n\nVilken plats skall tas bort (0 för ingen)?") - 1;

      if (index != 0)
	{
	  list_value_t val = { .p = NULL };
	  list_remove(selected.item->shelves, index, val);
	}
    }
  else
    {
      tree_remove(tree, (tree_key_t)  { .p = selected.name } );
    }
  
  action->type = REMOVE;
  action->saved = selected;
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
  list_clear(action->original.shelves, &free_shelf);

  // Copy shelves
  int shelves_length = list_length(shelves);
  for (int i = 0; i < shelves_length; i++)
    {
      shelf_t *tmp = (shelf_t *) list_get(shelves, i).p;
      list_value_t tmpshelf = { .p = make_shelf(strdup(tmp->name), tmp->amount) };
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
	  shelf_t *tmp = (shelf_t *) list_get(shelves, i).p;
          printf("%d. Namn:  %s\n", i+1, tmp->name);
          printf("   Mängd: %d\n\n", tmp->amount);

        }
      int index_shelf = ask_question_int("Vilken hylla vill du ändra?");
      shelf_t *tmp_shelf = (shelf_t *) list_get(shelves, index_shelf).p;
      tmp_shelf->name = ask_question_shelf("Ny hylla: ");
      break;
      

      // Edit the amount  
    case 'T':
      for (int i = 0; i < shelves_length; ++i)
	{
	  shelf_t *tmp = (shelf_t *) list_get(shelves, i).p;
          printf("%d. Namn:  %s\n", i+1, tmp->name);
          printf("   Mängd: %d\n\n", tmp->amount);
          
        }
      int index_amount = ask_question_int("Vilket antal vill du ändra?");
      shelf_t *tmp_amount = (shelf_t *) list_get(shelves, index_amount).p;
      tmp_amount->amount = ask_question_int("Nytt antal: ");
      break;
      
    case 'A':
      return;
    default:
      break;
    }
}
