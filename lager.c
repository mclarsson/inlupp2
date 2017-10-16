
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "tree.h"
#include "list.h"
#include "db.h"

/// Exists program
///
void exit_program()
{  
  puts("Avslutar program");
  exit(0);
}

/// Prints out main  menu
///
void print_main_menu()
{
  fputs("\n", stdout);
  fputs("[L]ägga till en vara\n", stdout);
  fputs("[T]a bort en vara \n", stdout);
  fputs("[R]edigera en vara\n", stdout);
  fputs("Ån[g]ra senaste ändringen\n", stdout);
  fputs("Lista [h]ela varukatalogen\n", stdout);
  fputs("[A]vsluta\n", stdout);
  fputs("\n", stdout);
}

/// Compare function for two strings
int cmp_goods_names(tree_key_t e1, tree_key_t e2)
{
  char *a = (char *) e1.p;
  char *b = (char *) e2.p;
  return strcmp(a, b);
}

/// Main program
///
int event_loop()
{
  tree_cmp_t *cmp = calloc(1, sizeof(tree_cmp_t));
  *cmp = &cmp_goods_names;
  
  tree_t *catalog = tree_new(cmp);
  action_t *act = action_new();
  
  puts("\n\n Välkommen till lagerhantering");
  puts(" ================================= \n");
  
  char input;
  
  while (true)
    {
      print_main_menu();
      input = ask_question_char_in_str(" > ", "LHTRGA");

      switch (input)
	{
	case 'L':
	  // Add item
	  add_goods(catalog, act);
	  break;

	case 'H':
	  // List catalog
	  list_goods(catalog);
	  break;

	case 'T':
	  //remove goods
	  remove_goods(catalog, act);
	  break;

	case 'R':
	  //edit goods
	  edit_goods(catalog, act);
	  break;

	case 'G':
	  //undo
	  undo_action(catalog, act);
	  break;
	         	  
	case 'A':
	  // Exit
	  free(cmp);
	  tree_delete(catalog, &free_goods);
	  free_action(act);
	  exit_program();
	  break;

	default:
	  fprintf(stdout, "%c är inte ett kommando \n", input);
	  break;
	}      
    }
}

int main (void)
{
  // Start program
  return event_loop();
}
