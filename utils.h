#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef union {
    int i;
    float f;
    char *s;
  } answer_t;


typedef bool(*check_func)(char *);
typedef answer_t(*convert_func)(char *);

extern char *strdup(const char *);

void clear_input_buffer();
int read_string(char *buf, int buf_siz);
bool is_number(char *str);
bool is_float(char *str);
answer_t make_float(char *);
bool not_empty(char *str);
answer_t ask_question(char *question, check_func check, convert_func convert);
char *ask_question_string(char *question);
int ask_question_int(char *question);
double ask_question_float(char *question);
char *ask_question_shelf(char *question);
char ask_question_char(char *question);
char ask_question_char_in_str(char *question, char *str);
void print_menu();
char ask_question_menu();
void print_edit_menu();
char ask_question_edit_menu();
char ask_question_add();

#endif
