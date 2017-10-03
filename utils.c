
#include "utils.h"

/**
 
 * not_empty
 
 */
bool not_empty(char *str)
{
  return strlen(str) > 0;
}

/**
 
 * is_float

 */
bool is_float(char *str)
{
  int len = strlen(str);
  bool hasPoint = false;

  // Negative numbers or start with . (.132)
  if (str[0] != '-' && str[0] != '.' && !isdigit(str[0])) {
    return false;
  }
  
  for (int i = 1; i < len; i++) {
    if (str[i] == '.') {
      // All floats have a .
      hasPoint = true;
    } else if (!isdigit(str[i])) {
      return false;
    }
  }
  
  return hasPoint;
}

/**
 
 * is_number
 
 */
bool is_number(char *str) {

  int len = strlen(str);

  // Negative numbers
  if (str[0] != '-' && !isdigit(str[0])) {
    return false;
  }
  
  for (int i = 1; i < len; i++) {
    if (!isdigit(str[i])) {
      return false;
    }
  }
  
  return true;
}

/**
 
 * clear_input_buffer
 
 */
void clear_input_buffer()
{
  int c;
  do {
    c = getchar();
  } while (c != '\n' && c != EOF);
}

/**
 
 * read_string
 
 */
int read_string(char *buf, int buf_siz)
{
  int i = 0;
  int max = buf_siz - 1;
  int c;

  // Fill buffer
  while (i < max) {
    c = getchar();
    
    if (c != '\n' && c != EOF) {
      buf[i] = c;
      i++;
    } else {
      break;
    }
  }

  // String to long for buffer size
  if (i == max) {
    clear_input_buffer();
  }

  buf[i] = '\0';

  return i;
}

/**
 
 * make_float
 
 */
answer_t make_float(char *str)
{
  return (answer_t) { .f = atof(str) };
}

/**
 
 * ask_question
 
 */
answer_t ask_question(char *question, check_func check, convert_func convert)
{
  int str_size = 255;
  char str[str_size];
  int length = 0;
  bool passed = false;

  do {
    puts(question);
    length = read_string(str, str_size);

    if (check(str))
      {
	passed = true;
      }
    else
      {
	fprintf(stdout, "Felaktig inmatning: %s \n", str);
      }
    
  } while(!passed || length == 0);
  
  return convert(str);
    
}

/**
 
 * ask_question_int
 
 */
int ask_question_int(char *question)
{
  return ask_question(question, is_number, (convert_func) atoi).i;
}

/**
 
 * ask_question_float
 
 */
double ask_question_float(char *question)
{
  return ask_question(question, is_float, make_float).f;
}

/**
 
 * ask_question_string
 
 */
char *ask_question_string(char *question/*, char *buf, int buf_siz*/)
{
  return ask_question(question, not_empty, (convert_func) strdup).s;
}

bool is_shelf(char *str)
{
  // check if first character is letter
  if (!isupper(str[0]) || strlen(str) <= 1) {
    return false;
  } else {
    // Check if all other characters are digits
    for (int i = 1; i < strlen(str); i++) {
      if (!isdigit(str[i])) {
	return false;
      }
    }

    // passed all checks
    return true;
  }
}

/**
 * ask_question_shelf
 */
char *ask_question_shelf(char *question)
{
  return ask_question(question, is_shelf, (convert_func) strdup).s;
}

char ask_question_char(char *question)
{
  printf("%s ", question);
  char c = fgetc(stdin);
  puts("");
  clear_input_buffer();
  return c;
}


/// Asks for char that is in string
///
char ask_question_char_in_str(char *question, char *str)
{
  char c;
  
  do {
    c = tolower(ask_question_char(question));
  } while (strchr(str, c) == NULL);
  
  return toupper(c);
}

/**
 
 * print
 
 */
void print (char *str)
{
  while (*str) {
    putchar(*str);
    str++;
  }
}

/**
 
 * println
 
 */
void println (char *str)
{
  print(str);
  putchar('\n');
}

/**
 
 * Swap
 
 */
void swap(int *a, int *b)
{
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

void print_menu()
{
  fputs("\n", stdout);
  fputs("[L]ägga till en vara\n", stdout);
  fputs("[T]a bort en vara \n", stdout);
  fputs("[R]edigera en vara\n", stdout);
  fputs("Ån[g]ra senaste ändringen\n", stdout);
  fputs("Lista [h]ela varukatalogen\n", stdout);
  fputs("[A]vsluta\n\n", stdout);
}

char ask_question_menu()
{
 print_menu();

  char menu_items[] = "LlTtRrGgHhAa";
  char c = 'c';

  
  do {
    // Ask for c while c is not in menu_items
    c = ask_question_char("Vad vill du göra?");
  } while (strchr(menu_items, c) == NULL);
  
  return toupper(c);
}

void print_edit_menu()
{
  fputs("[B]eskrivning\n", stdout);
  fputs("[P]ris\n", stdout);
  fputs("[L]agerhylla\n", stdout);
  fputs("An[t]al\n", stdout);
}

void print_add_menu()
{
  fputs("\n[J]a\n",stdout);
  fputs("[N]ej\n",stdout);
  fputs("[R]edigera\n",stdout);
}

char ask_question_edit_menu()
{
  print_edit_menu();

  char menu_items[] = "BbPpLlTtAa";
  char c = 'c';
  
  do {
    // Ask for c while c is not in menu_items
    c = ask_question_char("Välj rad eller [a]vbryt");
  } while (strchr(menu_items, c) == NULL);
  
  return toupper(c);
}

char ask_question_add()
{
  print_add_menu();

  char menu_items[] = "JjNnRr";
  char c = 'c';
  
  do {
    // Ask for c while c is not in menu_items
    c = ask_question_char("Välj rad eller [a]vbryt");
  } while (strchr(menu_items, c) == NULL);
  
  return toupper(c);
}
