
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
  item_t item;
  shelf_t shelf;
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

action_t *action_new()
{
  action_t *new = calloc(1, sizeof(action_t));
  new->original.shelves = list_new();
  new->type = NOTHING;
  return new;
}

bool shelf_exists(tree_t *tree, char *shelf)
{
  L *products = tree_elements(tree);
  int no_products = tree_size(tree);

  for(int i = 0; i < no_products; i++)
    {
      if(products[i] != NULL)
	{
	  item_t *tmp_item = products[i];
	  list_t *shelves = tmp_item->shelves;
	  int shelves_length = list_length(shelves);
	  for(int x = 0; x < shelves_length; x++)
	    {
	      shelf_t *tmp_shelf = list_get(shelves, x);
	      if(strcmp(tmp_shelf->name, shelf) == 0)
		{
		  return true;
		}
	    }
	}
      else
	{
	  return false;
	}
    }
  
  return false;
}

void print_item(item_t *item, bool newItem, char name[])
{
  printf("Namn: %s \nBeskrivning: %s \nPris : %d \n", name, item->description, item->price);
  list_t *shelves = item->shelves;
  if(newItem)
    {
      shelf_t *shelf = list_last(shelves);
      printf("Hylla: %s \nAntal: %d\n", shelf->name, shelf->amount);
    }
  else
    {
      for(int i = 0; i < list_length(shelves); i++)
	{
	  shelf_t *tmp_shelf = list_get(shelves, i);
	  printf("Hylla: %s \nAntal: %d\n", tmp_shelf->name, tmp_shelf->amount);
	}
    }
}

/// Creates new shelf
///
/// \param name Name of shelf (shelf format expected)
/// \param amount Amount of items on shelf
/// \returns pointer to shelf
shelf_t *make_shelf(char *name, int amount)
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
/// \returns pointer to item.
item_t *make_item(char *description, int price)
{
  item_t *new = calloc(1, sizeof(item_t));
  new->description = strdup(description);
  new->price = price;
  new->shelves = list_new();
  return new;
}

/// Adds shelf to item
///
/// \param item Item to add to
/// \param name Name of shelf
/// \param amount Amount of item on shelf
void add_shelf(item_t *item, char *name, int amount)
{
  shelf_t *new = make_shelf(name, amount);
  list_append(item->shelves, new);
}

item_t *input_item(tree_t *tree)
{
  char description[255];
  int price;
  char shelf[255];
  int amount;

  strcpy(description, ask_question_string("Beskrivning:") );
  price = ask_question_int("Pris:");
  
  item_t *item = make_item(description, price);
  do
    {
      strcpy(shelf, ask_question_shelf("Välj en hylla, den får inte redan användas i systemet"));
    } while(shelf_exists(tree, shelf));
  
  amount = ask_question_int("Hur många antal av varan finns det?");

  shelf_t *new_shelf = make_shelf(shelf, amount);
  list_append(item->shelves, new_shelf);
  
  return item;
}

void input_existing_item(list_t *shelves, char *shelf, int amount)
{

  for(int i = 0; i < list_length(shelves); i++)
    {
      shelf_t *tmp_shelf = list_get(shelves, i);
      if(strcmp(tmp_shelf->name, shelf))
	{
	  tmp_shelf->amount += amount;
	  return;
	}
    }

  shelf_t *new_shelf = make_shelf(shelf, amount);
  list_append(shelves, &new_shelf);
  
  return;
}

void remove_goods(tree_t *tree)
{
  return;
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
      while (list_remove(action->edited->shelves, 0, NULL));
      
      int shelves_length = list_length(action->original.shelves);

      // Add saved shelves
      for (int i = 0; i < shelves_length; ++i)
	{
	  shelf_t *tmp = list_get(action->original.shelves, i);
	  list_append(action->edited->shelves, tmp); 
	}
      
      // Remove all saved shelves 
      while (list_remove(action->original.shelves, 0, NULL));
    }
  else if (action->type == ADD)
    {
      puts("Kan inte ångra lägga till");
    }
  else if (action->type == NOTHING)
    {
      puts("Inget att ångra");
    }
  
  // Prevents multiple regrets
  action->type = NOTHING;
}

void edit_base_item(tree_t *tree, item_t *item)
{
  char input = ask_question_edit_menu();
  list_t *shelves = item->shelves;
     
  switch(input)
    {	  
    case 'B':
      printf("\nNuvarande beskrivning: %s\n", item->description);
      strcpy(item->description, ask_question_string("Vad vill du ändra beskrivningen till?"));
      break;

    case 'P':
      printf("\nNuvarande pris: %d\n", item->price);
      item->price = ask_question_int("Vad vill du ändra priset till?");
      break;

    case 'L':
      {
	shelf_t *base_shelf_shelfname = list_first(shelves);
	printf("\nNuvarande lagerhylla: %s \n", base_shelf_shelfname->name);
	char *tmp_shelf = "Z9999";
	tmp_shelf = ask_question_shelf("Vad vill du ängra lagerhyllan till? Du får inte välja en som redan finns.");
	
	while(shelf_exists(tree, tmp_shelf))
	  {
	    tmp_shelf = ask_question_shelf("Vad vill du ängra lagerhyllan till? Du får inte välja en som redan finns.");
	  }
	strcpy(base_shelf_shelfname->name, tmp_shelf);
	break;
      }

    case 'T':
      {
	shelf_t *base_shelf_amount = list_first(shelves);
	printf("\nNuvarande lagerhylla: %s\nNuvarande antal: %d\n", base_shelf_amount->name, base_shelf_amount->amount);
	int tmp_amount = ask_question_int("Vad vill du ändra antalet till?");
	base_shelf_amount->amount = tmp_amount;
	break;
      }

    case 'A':
      {
	return;
      }

    default:
      puts("error");
      break;
    }
}

void add_goods(tree_t *tree, action_t *action)
{
  char name[255];
  item_t *item;
  char shelf[255];
  int amount;
  strcpy(name, ask_question_string("Namn:"));

  action->type = ADD;
  
  if(tree_has_key(tree, name))
    {
      puts("Varan finns redan, använder samma beskrivning & pris!");
      do
	{
	  strcpy(shelf, ask_question_shelf("Vilken hylla ska den ligga på?"));
	} while(shelf_exists(tree, shelf));
      amount = ask_question_int("Hur många varor?");
      item = tree_get(tree, name);
      add_shelf(item, shelf, amount);
    }
  else
    {
      item = input_item(tree);
       }

  while(true)
    {
      print_item(item, true, name);
      fputs("Vill du spara varan i databasen?",stdout);
      char input = ask_question_add();

      switch(input)
	{
	case 'J':
	  if(tree_has_key(tree, name))
	    {
	      item = tree_get(tree, name);
	      list_t *shelves = item->shelves;
	      input_existing_item(shelves, shelf, amount);
	    }
	  else
	    {
	      tree_insert(tree, name, item);
	    }
	case 'N':
	  return;
	case 'R':
	  edit_base_item(tree, item);
	default:
	  break;
	}
    }
}

char *select_goods(tree_t *tree)
{
  //int size = tree_size(tree);
  int page_size = 20;
  //int pages_to_view = 1;
  K *products = tree_keys(tree);
  char *input;
  int current_page = 0;
  char *opt1 = "N";
  char *opt2 = "F";
  char *opt3 = "A";
  int input_n;
  int product_size = tree_size(tree);

  while(true)
    {
      input = "";
      input_n = -1;

      for(int i = 0+(current_page*page_size); i < page_size+(current_page*page_size); i++)
	{
	  if(products[i] == NULL || i >= product_size)
	    {
	      break;
	    }
	  else
	    {
	      printf("%d %s\n", (i%page_size)+1, products[i]);
	    }
	}

      while(strcmp(input, opt1) != 0 && strcmp(input, opt2) != 0 && strcmp(input, opt3) != 0 && is_number(input) == false)
	{
	  input = ask_question_string("\nVälj en vara (nummer)\n[N]ästa sida\n[F]örra sidan\n[A]vbryt");
	  if(is_number(input) == false)
	    {
	      input[0] = toupper(input[0]);
	    }
	}
	
      if(is_number(input))
	{
	  input_n = atoi(input);
	  if(input_n > 0 && input_n <= page_size && products[input_n+(page_size*current_page)-1] != NULL)
	    {
	      char *name = strdup(products[input_n+(page_size*current_page)-1]);
	      return name;
	    }
	}
      else if(strcmp(input, opt1) == 0)
	{
	  if((current_page+1)*page_size < product_size)
	    {
	      current_page++;
	    }
	  else
	    {
	      puts("Det finns ingen nästa sida");
	    }
	}
      else if(strcmp(input, opt2) == 0)
	{
	  if(current_page > 0)
	    {
	      current_page--;
	    }
	  else
	    {
	      puts("Det här är första sidan");
	    }
	}
      else
	{
	  return NULL;
	}
    }
}

/// Outputs entire catalog in list format.
///
/// \param tree Tree containing catalog.
void list_goods(tree_t *tree)
{
  int size = tree_size(tree);
  K *items = tree_keys(tree);
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
      if (current_page * page_size > size)
	{
	  max = size - index;
	}
      else
	{
	  max = page_size;
	}
      
      for (int k = 1; k <= max; ++k, ++index)
	{
	  printf("%d.\t%s\n", k, items[index]);
	}

      // Only ask for next page if not all items have been listed

      // User entered an index between 1-20
      bool valid_input = false;

      // User entered A or N
      bool valid_index = false;
      
      do {
	char *input = ask_question_string("\nSe vara [1-20], [n]ästa sida eller [a]vbryt");
	
	if (is_number(input))
	  {
	    int input_index = atoi(input);
	    
	    if (input_index > 0 && input_index <= max)
	      {
		int item_index  = (input_index - 1) + (current_page - 1) * page_size;
		item_t *item = tree_get(tree, items[item_index]);

		printf("\nNamn: %s \n", items[item_index]);
		printf("Beskrivning: %s \n", item->description);
		printf("Pris: %d \n", item->price);

		int shelves_length = list_length(item->shelves);
		for (int i = 0; i < shelves_length; ++i)
		  {
		    shelf_t *s = list_get(item->shelves, i);
		    printf("Hylla: %s \n", s->name);
		    printf("Antal: %d \n\n", s->amount);
		  }

		valid_index = true;

		// Repeat page
		index -= max;
		
	      }
	  }
	else
	  {
	    // [a]vbryt 
	    if (toupper(input[0]) == 'A')
	      {
		view_next = false;
		valid_input = true;
	      }
	    // [n]ästa sida
	    else if (toupper(input[0]) == 'N')
	      {
		valid_input = true;
		++current_page;
	      }
	  }
	
      } while (valid_input == false && valid_index == false);
    } 
}

void display_goods(tree_t *tree)
{
  char *goods = select_goods(tree);
  item_t *item = tree_get(tree, goods);
  list_t *shelves = item->shelves;
  for(int i = 0; i < list_length(shelves); i++)
    {
      shelf_t *tmp_shelf = list_get(shelves, i);
      printf("\nName: %s \nBeskrivning: %s \nPris : %d \nHylla: %s \nAntal: %d", goods, item->description, item->price, tmp_shelf->name, tmp_shelf->amount);
    }
}

void edit_goods(tree_t *tree, action_t *action)
{
  char *name = select_goods(tree);
  if(name == NULL)
    {
      return;
    }
  
  item_t *tmp_item = tree_get(tree, name);
  list_t *shelves = tmp_item->shelves;

  // Copy item for undo

  action->type = EDIT;
  action->edited = tmp_item;
  action->original.description = strdup(tmp_item->description);
  action->original.price = tmp_item->price;

  // Clear copys shelves
  while (list_remove(action->original.shelves, 0, NULL));

  // Copy shelves
  int shelves_length = list_length(shelves);
  for (int i = 0; i < shelves_length; i++)
    {
      shelf_t *tmp = list_get(shelves, i);
      list_append(action->original.shelves, make_shelf(tmp->name, tmp->amount)); 
    }
  
  char input = ask_question_edit_menu();
  
  switch(input)
    {	  
    case 'B':
      printf("Nuvarande beskrivning: %s\n", tmp_item->description);
      strcpy(tmp_item->description, ask_question_string("Vad vill du ändra beskrivningen till?"));
      break;

    case 'P':
      printf("Nuvarande pris: %d\n", tmp_item->price);
      tmp_item->price = ask_question_int("Vad vill du ändra priset till?");
      break;

    case 'L':
      {
	for(int i = 0; i < list_length(shelves); i++)
	  {
	    shelf_t *base_shelf_shelfname = list_get(shelves, i);
	    printf("Nuvarande lagerhylla: %s \n", base_shelf_shelfname->name);
	    char *tmp_shelf;
	
	    do
	      {
		tmp_shelf = ask_question_shelf("Vad vill du ängra lagerhyllan till? Du får inte välja en som redan finns.");
	      } while(shelf_exists(tree, tmp_shelf) && strcmp(tmp_shelf, base_shelf_shelfname->name));
	    strcpy(base_shelf_shelfname->name, tmp_shelf);
	  }
	break;
      }

    case 'T':
      {
	for(int i = 0; i < list_length(shelves); i++)
	  {
	    shelf_t *base_shelf_amount = list_get(shelves, i);
	    printf("Nuvarande lagerhylla: %s\nNuvarande antal: %d\n", base_shelf_amount->name, base_shelf_amount->amount);
	    int tmp_amount = ask_question_int("Vad vill du ändra antalet till?");
	    base_shelf_amount->amount = tmp_amount;
	  }
	break;
      }

    case 'A':
      {
	return;
      }

    default:
      puts("error");
      break;
    }
}
