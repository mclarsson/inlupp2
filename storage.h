#ifndef __storage_h__
#define __storage_h__

#include <stdio.h>

/// Opens file if file exists, creates it otherwise.
/// File can be both read and written to.
///
/// \param name filename
/// \returns: file
FILE *open_file(char *name);

/// Closes file
///
/// \param file file to close
void close_file(FILE *file);

/// Stores value of a certain size in file
///
/// \param file file to write to
/// \param value pointer to value
/// \param size size of value
void store(FILE *file, void *value, int size);

/// Stores a string to file
///
/// \param file file to store in
/// \param str string to store
void store_string(FILE *file, char *str);

/// Stores an int to file
///
/// \param file file to store in
/// \param number int to store
void store_int(FILE *file, int number);

/// Loads a value into a holder
///
/// \param file file to load from
/// \param holder pointer to holder
/// \param size size of value in bytes
void load_value(FILE *file, void *holder, int size);

/// Loads string from file
///
/// \param file file to load from
/// \param length length of string
/// \returns: pointer to duplicated string
char *load_string(FILE *file, int length);

/// Loads int from file
///
/// \param file file to load from
/// \returns: loaded int
int load_int(FILE *file);

#endif
