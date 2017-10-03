#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#include "utils.h"


bool not_empty(char *str)
{
  return strlen(str) > 0;
}

void clear_input_buffer()
{
  int c;
  do
    {
      c = getchar();
    }
  while (c != '\n' && c != EOF);
}

int ask_question_int(char *question)
{
  answer_t answer = ask_question(question, is_number, (convert_func) atoi);
  return answer.i; // svaret som ett heltal
}

int read_string(char *buf, int buf_siz)
{
  int i = 0;
  int max = buf_siz -1;

  while (i < max)
    {
      int c;
      c = getchar();
    
      if (c  == EOF || c == '\n')
        {
          break;
        }
      else{    
        buf[i] = c;
        i++;
      } 
    }
  if (i == max){
    clear_input_buffer();
  }
  buf[i] = '\0';
  return i;
}

char *ask_question_string(char *question)
{
  return ask_question(question, not_empty, (convert_func) strdup).s;
}

int print (char *str){
  for (; *str != '\0'; str++)
    {
      putchar(*str);
    }
  return 0;
}

int println (char *word){
  print(word);
  print("\n");
  return 0;
}


bool is_shelf(char *shelf)
{
  if (!isalpha(shelf[0]))
    {
      printf("Felaktig inmatning!\n");
      return false;
    }
  shelf++;
  return is_number(shelf);
}


bool is_number(char *str){
  int size = strlen(str)-1;
  int offset = 0;
  
  if (str[0] == '-'){
    offset = 1;
  }
  
  for (int i = offset; i <= size; i++){
    if (!isdigit(str[i])){
      printf("Felaktig inmatning!\n");
      return false;
      }
    }
    return true;
}


answer_t ask_question(char *question, check_func check, convert_func convert)
{
  int buf_siz = 255;
  char buf[buf_siz];

  while (true)
    {
      puts(question);      
      int length = read_string(buf, buf_siz);
      if (check(buf) && length > 0)
        {
          return convert(buf);
        }  
    } 
}

char *ask_question_shelf(char *question)
{
  return ask_question(question, is_shelf, (convert_func) strdup).s;
}


bool check_char(char *question)
{
  if ((strlen(question) == 1) && (isalpha(question))) 
    {
      return true;
    }
  else
    {
      printf("Felaktig inmatning!\n");
      return false;
    }
}

char *ask_question_char(char *question)
{
  return ask_question(question, check_char, (convert_func) strdup).s;
  
}
