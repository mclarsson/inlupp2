#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include <time.h>
#include <ctype.h>



typedef struct item item_t;

struct item
{
  char *name;
  char *desc;
  int price;
  char *shelf;
};

void print_menu()
{
  printf("Välkommen till lagerhantering 1.0\n");
  printf("=================================\n\n");
  printf("[L]ägga till en vara\n");
  printf("[T]a bort en vara\n");
  printf("[R]edigera en vara\n");
  printf("Ån[g]ra senaste ändringen\n");
  printf("Lista [h]ela varukatalogen\n");
  printf("[A]vsluta\n\n");
  printf("Vad vill du göra idag? ");
}


void error_msg(char *fault)
{
  printf("Felaktig inmatning: '%s'\n", fault);
}


void print_item(item_t *item)
{
  int a = item->price/100;
  int b = item->price%100;
  printf("Name: %s\n", item->name);
  printf("Description: %s\n", item->desc);
  printf("Price: %d.%d Sek\n",a ,b );
  printf("Shelf: %s\n", item->shelf);

}

item_t  make_item(char *name, char *desc, int price, char *shelf)
{
  item_t item = {.name =name, .desc=desc, .price = price, .shelf = shelf};
  return item;

 
}
/*
bool is_shelf(char *shelf)
{
  if (!isupper(shelf[0]))
    {
      return false;
    }
  shelf++;
    return (is_number(shelf));
}

char *ask_question_shelf(char *question)
{
 
  return ask_question(question, is_shelf,(convert_func)strdup ).s;
  } 
*/


item_t input_item()
{
  char *inputName  = ask_question_string ("Input name of item: ");
  char *inputDesc  = ask_question_string ("Input description of item: ");
  int inputPrice   = ask_question_int    ("Input price: ");
  char *inputShelf = ask_question_shelf  ("Input shelf: ");
  char *what_to_do = ask_question_string ("Would you like to add that item to the database? (Yes / No / Edit) ");
  item_t tmp = make_item(inputName, inputDesc, inputPrice, inputShelf);
  

  if (strcmp(what_to_do,"Yes") == 0)
    {
      return tmp;
    }
  else if (strcmp(what_to_do, "No") == 0)
    {
      print_menu();
    }
  else if (strcmp(what_to_do, "Edit") == 0)
    {
      input_item();
    }
  else
    {
      printf("That is not a valid input, please try again!\n");
      
    }
  return (make_item(0,0,0,0));
 } 

char *magick (char *array1[], char *array2[], char *array3[],int length)
{
  char buf[255];
  int i = rand()%length;
  int n = rand()%length;
  int m = rand()%length;
  snprintf(buf, 255, "%s-%s %s", array1[i],array2[n],array3[m]);
  
    return strdup(buf);
}

void list_db(item_t *items, int no_items)
{
  for (int i = 0; i<no_items;i++)
    {
      printf("%d. %s\n", (i+1),items[i].name);
    }
}

void edit_db(item_t *item,int db_siz)
{
  int num = ask_question_int("Enter number of desired item: ");
  if (num >= 0 && num <= db_siz)
    {
      print_item(&item[num-1]);
      item[num-1] = input_item();
    }
  else
    {
      edit_db(item, db_siz);
    }
    
}
  

void add_item_to_db(item_t *item, int db_siz)
{
  item[db_siz] = input_item();
  return;
}

void remove_item_from_db(item_t *item, int db_siz)
{
  {
  int num = ask_question_int("Which item would you like to remove? ");
  num--;
  if (num >= 0 && num <= db_siz)
    {
       for (; num < db_siz; num++)
	{
	  item[num] = item[num+1];
        
	}
    }
  else
    {
      remove_item_from_db(item, db_siz);
    }
  return;
  }
}

void event_loop(item_t *item, int db_siz)
{
  while(true)
    {
    println("");
    print_menu();
    char val = toupper(fgetc(stdin));
    if (val == 'L')// || val == 'x')
      {
	clear_input_buffer();
	add_item_to_db(item, db_siz);
	++db_siz; // if 
      }
    else if (val == 'T')// || val == 't')
      {
	clear_input_buffer();
	remove_item_from_db(item, db_siz);
	--db_siz; // if 
      }
    else if (val == 'R')// || val == 'r')
      {
	clear_input_buffer();
	edit_db(item, db_siz);
      }
    else if (val == 'G')// || val == 'g')
      {
	clear_input_buffer();
	printf("Not yet implemented!\n");
      }
    else if (val == 'H')// || val == 'h')
      {
	clear_input_buffer();
	list_db(item, db_siz);
      }
    else if (val == 'A')// || val == 'a')
      {
	return;
      }
  }    
}
int main(int argc, char *argv[])
{
  srandom(time(NULL));
 char *array1[] = { "Laser",        "Polka",    "Extra" };
 char *array2[] = { "förnicklad",   "smakande", "ordinär" };
 char *array3[] = { "skruvdragare", "kola",     "uppgift" };


  if (argc < 2)
  {
    printf("Usage: %s number\n", argv[0]);
  }
  else
  {
    item_t db[16]; // Array med plats för 16 varor
    int db_siz = 0;    // Antalet varor i arrayen just nu

    int items = atoi(argv[1]); // Antalet varor som skall skapas

    if (items > 0 && items <= 16)
    {
      for (int i = 0; i < items; ++i)
      {
        // Läs in en vara, lägg till den i arrayen, öka storleksräknaren
        item_t item = input_item();
        db[db_siz] = item;
        ++db_siz;
      }
    }
    else
    {
      puts("Sorry, must have [1-16] items in database.");
      return 1; // Avslutar programmet!
    }

    for (int i = db_siz; i < 16; ++i)
      {
	///srand(time(NULL));
        char *name = magick(array1, array2, array3, 3); /// Lägg till storlek
        char *desc = magick(array1, array2, array3, 3); /// Lägg till storlek
        int price = rand() % 200000;
        char shelf[] = { rand() % ('Z'-'A') + 'A',
                         rand() % 100 + '0',
                         rand() % 10 + '0',
                         '\0' };
        item_t item = make_item(name, desc, price, strdup(shelf));

        db[db_siz] = item;
        ++db_siz;
      }


    event_loop(db, db_siz);

  }
  return 0;
}

