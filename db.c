
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
  element_t price;
  list_t *shelves;
};

struct shelf {
  char *name;
  element_t amount;
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
    struct { item_t saved; };      // REMOVE
    struct { item_t *edited; item_t original; }; // EDIT
  };
};


int cmp_ex(element_t e1, element_t e2)
{
  return e1.i < e2.i ? -1 : e1.i == e2.i ? 0 : 1;
}



/// Creates new action tracker
///
/// \returns: new action
action_t *action_new()
{
  cmp_t cmp_f = cmp_ex;
  
  action_t *new = calloc(1, sizeof(action_t));
  new->original.shelves = list_new(&cmp_f);
  new->type = NOTHING;
  return new;
}

/// Creates new shelf
///
/// \param name Name of shelf (shelf format expected)
/// \param amount Amount of items on shelf
/// \returns: pointer to shelf
shelf_t *make_shelf(char *name, element_t amount)
{
  shelf_t *new = calloc(1, sizeof(shelf_t));
  new->name = calloc(4, sizeof(char));
  sprintf(new->name, "%s", name);
  //new->name = name;
  new->amount = amount;
  return new;
}

/// Creates item
///
/// \param description Description of item.
/// \param price Price of item.
/// \returns: pointer to item.
item_t *make_item(char *description, element_t price)
{
  cmp_t cmp_f = &cmp_ex;
  item_t *new = calloc(1, sizeof(item_t));
  new->description = strdup(description);
  new->price = price;
  new->shelves = list_new(&cmp_f);
  return new;
}

/// Adds shelf to item
///
/// \param item Item to add to
/// \param name Name of shelf
/// \param amount Amount of item on shelf
void add_shelf(item_t *item, char *name, element_t amount)
{
  shelf_t *new = make_shelf(name, amount);
  element_t tmp = new->amount;
  list_append(item->shelves, tmp);
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
      action->edited->description = strdup(action->original.description);
      action->edited->price = action->original.price;

      // Remove all current shelves
      element_t null = { .p = NULL };
      while (list_remove(action->edited->shelves, 0, null));
      
      int shelves_length = list_length(action->original.shelves);

      // Add saved shelves
      for (int i = 0; i < shelves_length; ++i)
	{
	  shelf_t *tmpshelf = (shelf_t*) list_get(action->original.shelves, i).p;
          element_t tmpamount = tmpshelf->amount;
	  list_append(action->edited->shelves, tmpamount); 
	}
      
      // Remove all saved shelves
      while (list_remove(action->original.shelves, 0, null));
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
  output("Beskrivning", item->description);
  output_price("Pris", (item->price).i);

  int shelves_length = list_length(item->shelves);
  for (int i = 0; i < shelves_length; ++i)
    {
      shelf_t *s = (shelf_t *) list_get(item->shelves, i).p;
      output("Hylla", s->name);
      output_int("Antal", (s->amount).i);
    }
}

/// Check if shelf exists in system
///
/// \param tree the tree to be searched
/// \param name shelf name to be searched for
/// \returns: true if shelf is already in use, else false
bool shelf_exists(tree_t *tree, char *name)
{
  element_t *items = tree_elements(tree);
  int size = tree_size(tree);
  
  for (int i = 0; i < size; ++i)
    {
      item_t *item = items[i].p;
      list_t *shelves =  item->shelves;
      int shelf_length = list_length(shelves);
      
      for (int j = 0; j < shelf_length; ++j)
	{
	  element_t tmpshelf =  list_get(shelves, j);
          shelf_t *shelf = (shelf_t*) tmpshelf.p;
          

	  if (strcmp(name, shelf->name) == 0)
	    {
	      return true;
	    }
	}
    }

  return false;
}

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
	return strdup(name);
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
  element_t tmpamount = { .i = amount };
  return make_shelf(name, tmpamount);
}

/// Asks for inputs and creates item
///
/// \param tree the tree in which the item is created
/// \returns: new item
item_t *input_item(tree_t *tree)
{
  char *description = ask_question_string("Beskrivning:");
  int price = ask_question_int("Pris:");
  element_t tmpprice = { .i = price };
  item_t *item = make_item(description, tmpprice);
  shelf_t *shelf = input_shelf(tree);
  element_t tmpshelf = { .p = shelf };
  list_append(item->shelves, tmpshelf);
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
    char *name = ask_question_string("Namn:");
    element_t tmpname = { .p = name };
    
    if (tree_has_key(tree, tmpname))
      {
	puts("Varan finns redan, lägger till ny hylla");
	
	element_t elem =  tree_get(tree, tmpname);
        item_t *item = (item_t*) elem.p;
        

	// Ask for shelfname until it's either new or if it already belongs to item
	do {
	  char *shelf_name = ask_question_shelf("Hylla:");
	  
	  if (!shelf_exists(tree, shelf_name))
	    { 
	      // Add new shelf
	      int amount = ask_question_int("Antal:");
              element_t tmpamount = { .i = amount };
	      shelf_t *new_shelf = make_shelf(shelf_name, tmpamount);
              element_t tmpshelf = { .p = new_shelf };
	      list_append(item->shelves, tmpshelf);
	      save = true;
	    }
	  else
	    {
	      // Shelf exists, check if item has shelf
	      int shelf_length = list_length(item->shelves);
	      for (int i = 0; i < shelf_length; ++i)
		{
		  shelf_t *tmp = (shelf_t *) list_get(item->shelves, i).p;
		  if (strcmp(shelf_name, tmp->name) == 0)
		    {
		      // Aggregate shelf amount
		      tmp->amount.i += ask_question_int("Lägg till antal:");
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
	item_t *item = input_item(tree);
        element_t tmpitem = { .p = item };

	// Confirm item
	print_item(name, item);
	puts("Vill du lägga till?");
	char input = ask_question_char_in_str("[J]a, [n]ej, [r]edigera", "JNR");

	if (input == 'J')
	  {
	    tree_insert(tree, tmpname, tmpitem);
	    save = true;
	  }
	else if (input == 'N')
	  {
            element_t null = { .p = NULL };
	    list_remove(item->shelves, 0, null);
	    free(item->shelves);
	    free(item);
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
  element_t *items = tree_keys(tree);
  int page_size = 20;
  
  // Current index for iteration through items
  int index = 0;
  int current_page = 1;
  bool view_next = true;
  
  while (size > index && view_next)
    {
      printf("Sida: %d \n\n", current_page);
      
      // List items
      int max;
      if (current_page * page_size > size) max = size - index;
      else max = page_size;
      
      for (int k = 1; k <= max; ++k, ++index)
	{
	  printf("%d.\t%s\n", k, (char *)items[index].p);
	}

      char *input = ask_menu_option("\nVälj vara [1-20], [n]ästa sida eller [a]vbryt");
	
      if (is_number(input))
	{
	  
	  int input_index = atoi(input);
	    
	  int item_index  = (input_index - 1) + (current_page - 1) * page_size;
	  item_t *item = tree_get(tree, items[item_index]).p;

	  //print_item(items[item_index], item);
	  return (goods_t) { .name = strdup(items[item_index].p), .item = item };
	  
	  // Repeat page
	  index -= max;
	}
      else if (is(input, "A"))
	{
	  view_next = false;
	}
      else if(is(input, "N"))
	{
	  ++current_page;
	}
    }

  return (goods_t) { .name = NULL, .item = NULL };
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
  action->original.description = strdup(goods.item->description);
  action->original.price = goods.item->price;

  // Clear copys shelves
  element_t null = { .p = NULL };
  while (list_remove(action->original.shelves, 0, null));

  // Copy shelves
  int shelves_length = list_length(shelves);
  for (int i = 0; i < shelves_length; i++)
    {
      shelf_t *tmp = (shelf_t *) list_get(shelves, i).p;
      element_t tmpshelf = { .p = make_shelf(tmp->name, tmp->amount) };
      list_append(action->original.shelves, tmpshelf); 
    }

  puts("\n[B]eskrivning");
  puts("[P]ris");
  puts("[L]agerhylla");
  puts("An[t]al\n");

  char input = ask_question_char_in_str("Välj rad eller [a]vbryt", "BPLTA");

  switch (input)
    {
    case 'B':
      output("Nuvarande beskrivning", goods.item->description);
      puts("----------------------");
      goods.item->description = ask_question_string("Ny beskrivning: ");
      break;
      
    case 'P':
      output_price("Nuvarande pris", goods.item->price.i);
      puts("---------------");
      goods.item->price.i = ask_question_int("Nytt pris: ");
      break;
      
    case 'L':
      for (int i = 0; i < shelves_length; ++i)
	{
	  shelf_t *tmp = (shelf_t *) list_get(shelves, i).p;
	  
	  output("Nuvarande hylla", tmp->name);
	  puts("----------------");
	  tmp->name = ask_question_shelf("Ny hylla: ");

	  output_int("Nuvarande antal", tmp->amount.i);
	  puts("----------------");
	  tmp->amount.i = ask_question_int("Nytt antal: ");
	}
      break;
      
    case 'T':
      for (int i = 0; i < shelves_length; ++i)
	{
	  shelf_t *tmp = (shelf_t *) list_get(shelves, i).p;

	  output("Hylla", tmp->name);
	  output_int("Nuvarande antal", tmp->amount.i);
	  puts("----------------");
	  tmp->amount.i = ask_question_int("Nytt antal: ");
	}
      break;
      
    case 'A':
    default:
      return;
      break;
    }
}
