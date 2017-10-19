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



/// Checks if a string is empty or not
///
/// \param str string to be measured
/// \returns: true if string is not empty, else false
bool not_empty(char *str);


/// Checks if a string is a float number
///
/// \param str string to be converted and checked
/// \returns: true if string is a float number, else false
bool is_float(char *str);


/// Checks to see if a string is a number
///
/// \param str string to be checked
/// \returns: true if the string is a number, else false
bool is_number(char *str);


/// Clears the current input buffer
void clear_input_buffer();



int read_string(char *buf, int buf_siz);


answer_t make_float(char *);

answer_t ask_question(char *question, check_func check, convert_func convert);
char *ask_question_string(char *question);
int ask_question_int(char *question);
double ask_question_float(char *question);
char *ask_question_shelf(char *question);
char ask_question_char(char *question);
char ask_question_char_in_str(char *question, char *str);
char ask_question_menu();

char *ask_menu_option(char *question);


/// Prints out string in specific format: title, colon, tab, content, new line
/// [title]:    [content]
///
/// \param title string before colon
/// \param content string after colon
void output(char *title, char *content);

/// Prints out int in specific format: title, colon, tab, content, new line
/// [title]:    [content]
///
/// \param title string before colon
/// \param content int after colon
void output_int(char *title, int number);

/// Prints out price in kr.
///
/// \param title title of output
/// \param price price in ören
void output_price(char *title, int price);

#endif
