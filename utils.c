
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
  if (str[0] != '-' && str[0] != '.' && !isdigit(str[0]))
    {
      return false;
    }
  
  for (int i = 1; i < len; i++)
    {
      if (str[i] == '.')
        {
          // All floats have a .
          hasPoint = true;
        }
      else if (!isdigit(str[i]))
        {
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
  if (str[0] != '-' && !isdigit(str[0]))
    {
      return false;
    }
  
  for (int i = 1; i < len; i++)
    {
      if (!isdigit(str[i]))
        {
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
  while (i < max)
    {
      c = getchar();
    
      if (c != '\n' && c != EOF)
        {
          buf[i] = c;
          i++;
        }
    
      else
        {
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

/// Determines if string is in shelf format (A12)
///
/// \param str potential shelf to be checked
/// \returns: true if str is in correct format, false otherwise
bool is_shelf(char *str)
{
  // Checks if the string is a legit shelf of length 2
  if (isalpha(str[0]) && isdigit(str[1]) && strlen(str) < 3)
    {
      str++;
      return is_number(str);
    }

  // Checks if the string is a legit shelf of length 3
  else if (isalpha(str[0]) && isdigit(str[1]) && isdigit(str[2]) && strlen(str) < 4)
    {
      str++;
      return is_number(str);
    }

  // If the string is not a legit shelf
  else
    {
      return false;
    }
}


/// Asks for a shelf name
///
char *ask_question_shelf(char *question)
{
  return ask_question(question, is_shelf, (convert_func) strdup).s;
}

/// Asks for a char
///
char ask_question_char(char *question)
{
  printf("%s ", question);
  char c = fgetc(stdin);
  clear_input_buffer();
  return c;
}


/// Asks for char that is in string, not case sensitive
///
/// example:
///     ask_question_char_in_str("[J]a, [N]ej eller [A]vbryt", "JNA");
char ask_question_char_in_str(char *question, char *str)
{
  char c;
  
  do {
    c = toupper(ask_question_char(question));
  } while (strchr(str, c) == NULL);
  
  return toupper(c);
}


/// Returns the character inputed if it is in brackets in paramater (case insensitive)
///
/// example: ask_menu_option("[J]a, [N]ej eller [A]vbryt") can return
/// only when input is J/j, N/n, or A/a
char *ask_menu_option(char *menu)
{
  // Gettig menu items
  
  char *items[255];
  char *question = menu;
  int items_length = 0;
  while (*menu != '\0' && items_length < 255)
    {
      if (*menu == '[')
	{
	  // Skip current [
	  ++menu;
	  
	  char str[16] = "";
	  int str_length = 0;
	  while (*menu != ']' && str_length < 16)
	    {
	      str[str_length] = toupper(*menu);
	      ++str_length;
	      ++menu;
	    }
	  str[str_length] = '\0';
	  
	  items[items_length] = strdup(str);
	  ++items_length;
	}
      else
	{
	  ++menu;
	}
    }

  // Getting input

  int str_size = 16;
  char str[str_size];
  int length = 0;
  bool passed = false;

  do {
    puts(question);
    length = read_string(str, str_size);

    // Convert input to uppercase
    for (int i = 0; i < str_size; i++) str[i] = toupper(str[i]);

    // Check if input is valid
    for (int i = 0; i < items_length; i++)
      { 
	// Check if item contains -, in which case it's an interval
	if (strchr(items[i], '-') != NULL && is_number(str))
	  {
	    // Split interval into low and high boundaries
	    char low_str[8];
	    char high_str[8];
	    int str_index = 0;
	    int high_index = 0;

	    // Copy low boundary
	    for (; items[i][str_index] != '-'; ++str_index)
	      {
		low_str[str_index] = items[i][str_index];
	      }

	    // skip -
	    ++str_index;

	    // Copy high boundary
	    for (; items[i][str_index] != '\0'; ++str_index, ++high_index)
	      {
		high_str[high_index] = items[i][str_index];
	      }

	    int input_number = atoi(str);
	    int low = atoi(low_str);
	    int high = atoi(high_str);

	    // Make sure input is within interval
	    if (input_number >= low && input_number <= high)
	      {
		passed = true;
	      }
	  }
	else if (!strcmp(str, items[i]))
	  {
	    // Strings match
	    passed = true;
	  }
      }

    if (passed == false)
      {
	fprintf(stdout, "Felaktig inmatning: %s \n", str);
      }
    
  } while(!passed || length == 0);

  return strdup(str);
}

int main(void)
{
  char *test = ask_menu_option("Visa vara [1-20], [n]ej eller [a]vbryt");
  puts(test);
  
  return 0;
}
