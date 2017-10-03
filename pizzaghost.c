#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "tree.h"
#include "list.h"
#include "utils.c"

typedef struct item item_t;
typedef struct shelf shelf_t;
typedef struct list list_t;
typedef struct action action_t;

struct item
{
  char *name;
  char *desc;
  int price;
  list_t *shelves;
};

struct shelf
{
  char *shelf_name;
  int amount;
};

struct action
{
  enum { NOTHING, ADD, REMOVE, EDIT } type;
  item_t *original;
  item_t copy;

  shelf_t *original_shelf;
  shelf_t copy_shelf;
};


void print_menu()
{
  puts("Välkommen till lagerhantering 1.0");
  puts("=================================\n");
  puts("[L] Lägga till en vara");
  puts("[T] Ta bort en vara");
  puts("[R] Redigera en vara");
  puts("[G] Ångra senaste ändringen");
  puts("[H] Lista hela varukatalogen");
  puts("[A] Avsluta\n");
  printf("Vad vill du göra idag? ");
}


void error_msg(char *fault)
{
  printf("Felaktig inmatning: '%s'\n", fault);
}

void print_shelf (shelf_t *shelf)
{
  char *shelf_name = (shelf->shelf_name);
  printf("Shelf: %s\n", shelf_name);
  int amount = (shelf->amount);
  printf("Amount: %d\n", amount);
  return;
}

void print_item (item_t *item, shelf_t *shelf)
{
  char *name = (item -> name);
  printf("Name: %s\n", name);
  char *desc = (item -> desc);
  printf("Desc: %s\n", desc);
  int price = (item -> price);
  printf("Price: %d.%d\n", (price / 100), (price % 100));
  print_shelf(shelf);
  return;
}

shelf_t *make_shelf(char *shelf_name, int amount)
{
  shelf_t *new_shelf = calloc(1, sizeof(shelf_t));
  new_shelf->shelf_name = shelf_name;
  new_shelf->amount = amount;
  return new_shelf;
}


item_t *make_item(char *name, char *desc, int price, char *shelfName, int amount)
{
  item_t *new_item = calloc(1, sizeof(item_t));
  list_t *new_list = list_new();
  shelf_t *new_shelf = make_shelf(shelfName, amount);
  list_append(new_list, new_shelf);
  new_item->name = name;
  new_item->desc = desc;
  new_item->price = price;
  new_item->shelves = list_new();
  list_append(new_item->shelves, new_shelf);
  return new_item;
}


int print_twenty(tree_t *tree)
{
  list_t *newlist = list_new();
  appendKeyInorder(tree, newlist);
  int check = 1;
  int index = 0;
  int pagenumb = 0;
  int checkleft = tree_size(tree);
  while (check != 0 && check == 1)
    {
      if (tree_size(tree) == 0)
        {
          printf("Ingenting i varukorgen.\n");
          return 0;
        }
      else if (checkleft < 20)
        {
          for (int i = 0; i < checkleft; i++)
            {
              char *key = ((char*) list_get(newlist, index));
              ++index;
              printf("%d: %s\n", i+1, key);
            }
          break;
        }
      else
        {

          {
            for (int i = 0; i < 20; i++)
              {
                char *key = ((char*) list_get(newlist, index));
                ++index;
                printf("%d: %s\n", i+1, key);
              }
            checkleft = checkleft - 20;
            check = ask_question_int("Vill du se ytterligare varor? Klicka 1 för ja och 0 för nej.\n");
            if (check == 1)
              {
                pagenumb = pagenumb + 20;
              }
          }
        }
    }
  return (pagenumb);

}


void show_item(tree_t *tree, list_t *newlist, int pagenumb)
{
  if (tree_size(tree) == 0)
    {
      return;
    }
  else
    {
  int index = ask_question_int("Skriv in det önskade indexet: ");
  appendInorder(tree, newlist);
  item_t *item = ((item_t*) list_get(newlist, (index - 1) + pagenumb));
  printf("Namn:         %s\n", item->name);
  printf("Beskrivning:  %s\n", item->desc);
  printf("Pris:         %d.%d kr\n", (item->price)/100, (item->price)%100);
  printf("Hylla:        %s\n", ((shelf_t*) list_get(item->shelves, index-1))->shelf_name);
  printf("Mängd:        %d\n\n", ((shelf_t*) list_get(item->shelves, index-1))->amount);
    }
}


item_t *addHelp(item_t *item, char *what_to_do)
{
  char wtd;
  
  if (strcmp(what_to_do, "Ja") == 0)
    {
      wtd = 'Y';
    }
  else if (strcmp(what_to_do, "Nej") == 0)
    {
      wtd = 'N';
    }
  else if (strcmp(what_to_do, "Redigera") == 0)
    {
      wtd = 'E';
    }
  else
    {
      wtd = 'I';
    }

  switch(wtd)
    {
    case 'Y' :
      {
        return item;
      }

    case 'N' :
      {
        break;
      }

    case 'E' :
      {
        puts("Needs to be implemented!");
      }

    case 'I' :
      {
        printf("Felaktig input, försök igen\n");
        break;
      }  
    default :
      {
        printf("Felaktig input, försök igen\n");
        break;
      }
    }
  return item;
}

bool shelf_exist(char *shelfname, list_t *newlist)
{
  for (int i = 0; i < list_length(newlist); ++i)
    {
      if ( strcmp((char *) list_get(newlist, i),shelfname) == 0)
        {
          return true;
        }
    }
  return false;
}

item_t *input_item(tree_t *tree, list_t *newlist)
{
  puts("");
  char *inputName  = ask_question_string ("Skriv in varans namn: ");
 HYLLA:
  if (tree_has_key(tree, inputName))
    {
      char *inputShelf = ask_question_shelf  ("Skriv in varans hylla: ");
      if (shelf_exist(inputShelf, newlist))
        {
          puts("Den hyllan används redan, försök igen!");
          goto HYLLA;
        }
      else
        {
          list_append(newlist, inputShelf);
          int inputAmount  = ask_question_int    ("Skriv in mängd av varan: ");
          char *what_to_do = ask_question_string ("Vill du lägga till varan i databasen? (Ja / Nej / Redigera) ");
          item_t *tmp = tree_get(tree, inputName);
          item_t *item = make_item(tmp->name, tmp->desc, tmp->price, inputShelf, inputAmount);
          addHelp(item, what_to_do);
        }
    }
  else
    {
      
      char *inputDesc  = ask_question_string ("Skriv in varans beskrivning: ");
      int inputPrice   = ask_question_int    ("Skriv in varans pris (i ören): ");
      char *inputShelf = ask_question_shelf  ("Skriv in varans hylla: ");
      list_append(newlist, inputShelf);
      int inputAmount  = ask_question_int    ("Skriv in mängd av varan: ");
      char *what_to_do = ask_question_string ("Vill du lägga till varan i databasen? (Ja / Nej / Redigera) ");
      item_t *item = make_item(inputName, inputDesc, inputPrice, inputShelf, inputAmount);
      addHelp(item, what_to_do);

    }
}


bool input_helper(tree_t **tree, item_t *item)
{
  return tree_insert(tree, item->name, item);
}


action_t edit_item(tree_t *tree, list_t *newlist, int input)
{
  appendInorder(tree, newlist);
  item_t *item = ((item_t*) list_get(newlist, input - 1));
  action_t newaction;
  memcpy(&(newaction.copy), item, sizeof(item_t));
  newaction.type = 3;
  newaction.original = item;
      
  puts("\n[B] Beskrivning");
  puts("[P] Pris");
  puts("[L] Lagerhylla");
  puts("[T] Antal\n");
  printf("Välj rad eller [A] avbryt: ");
  while(true)
    {
      char opt = toupper(fgetc(stdin));
      if (opt == 'B')
        {
          clear_input_buffer();
          printf("Nuvarande beskrivning: %s\n", item->desc);
          puts("---------------------------------------------");
          char *newDesc = ask_question_string("Ny beskrivning: ");
          item->desc = newDesc;
          return newaction;
        }
      else if (opt == 'P')
        {
          clear_input_buffer();
          printf("Nuvarande pris: %d.%d kr\n", (item->price)/100, (item->price)%100);
          puts("---------------------------------------------");
          int newPrice = ask_question_int("Nytt pris: ");
          item->price = newPrice;
          return newaction;
        }
      else if (opt == 'L')
        {
          clear_input_buffer();
          printf("Nuvarande hylla: %s\n", ((shelf_t*)list_get(item->shelves, input - 1))->shelf_name);
          puts("---------------------------------------------");
          char *newShelf = ask_question_shelf("Ny hylla: ");
          (((shelf_t*)list_get(item->shelves, input - 1))->shelf_name) = newShelf;
          return newaction;
        }
      else if (opt == 'T')
        {
          clear_input_buffer();
          printf("Nuvarande mängd: %d\n", ((shelf_t*)list_get(item->shelves, input - 1))->amount);
          puts("---------------------------------------------");
          int newAmount = ask_question_int("Ny mängd: ");
          (((shelf_t*)list_get(item->shelves, input - 1))->amount) = newAmount;
          return newaction;
        }
      else if (opt == 'A')
        {
          clear_input_buffer();
          break;
        }
      else
        {
          puts("Invalid input!");
        }
    }
  return newaction;
}


void undoEdit(action_t undo)
{
  if (undo.type == 0)
    {
      puts("Det finns inget att ångra!");
    }

  else if (undo.type == 1)
    {
      puts("Inte implementerat än!");
    }

  else if (undo.type == 2)
    {
      puts("Inte implementerat än!");
    }

  else if (undo.type == 3)
    {
      memcpy(undo.original, &(undo.copy), sizeof(item_t));
      undo.copy.desc                = strdup(undo.copy.desc);
      for (int i = 0; i < list_length(undo.copy.shelves)-1; ++i)
      {
      ((shelf_t*) list_get(undo.copy.shelves, i))->shelf_name = strdup(((shelf_t*) list_get(undo.copy.shelves, i))->shelf_name);
      }
      //memcpy(undo.copy.shelves->amount, undo.copy.shelves->amount, sizeof(item_t));
      }

  else
    {
      puts("Felaktig inmatning av data!");
    }
}


void event_loop()
{
  tree_t *newtree = NULL;
  list_t *newlist = list_new();
  action_t undo =  { 0, NULL, {}, NULL, {} };
  int index = 0;
  while(true)
    {
      print_menu();
      char val = toupper(fgetc(stdin));
      
      if (val == 'L')
        {
          clear_input_buffer();
          item_t *newitem = input_item(newtree, newlist);
          input_helper(&newtree, newitem);
        }
    
      else if (val == 'T')
        {
          clear_input_buffer();
          puts("Inte implementerat än\n!");
        }
    
      else if (val == 'R')
        {
          clear_input_buffer();
          puts("Lista av alla varor");
          puts("===================");
          int pagenumb = print_twenty(newtree);	
          puts("");
          show_item(newtree, newlist, pagenumb);
          undo = edit_item(newtree, newlist, index);
        }
    
      else if (val == 'G')
        {
          clear_input_buffer();
          undoEdit(undo);
        }
    
      else if (val == 'H')
        {
	clear_input_buffer();
        puts("Lista av alla varor");
        puts("===================");
        int pagenumb = print_twenty(newtree);
        puts("");
        show_item(newtree, newlist, pagenumb);
        }
    
      else if (val == 'A')
        {
          return;
        }
    }    
}

int main()
{
  event_loop();
}
