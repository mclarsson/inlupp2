#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdbool.h>

typedef union{
  int i;
  float f;
  char *s;
}answer_t;

typedef bool(*check_func)(char *);
typedef answer_t(*convert_func)(char *);

extern char *strdup(const char *);

void clear_input_buffer();
int read_string(char *buf, int buf_siz);
bool is_number(char *str);
bool not_empty(char *str);
answer_t ask_question(char *question, check_func check, convert_func convert);
char *ask_question_string(char *question);
char *ask_question_shelf(char *question);
int ask_question_int(char *question);
int print(char *word);
int println(char *word);
char *ask_question_char(char *question);
#endif
