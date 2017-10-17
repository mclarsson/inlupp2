
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// Opens file if file exists, creates it otherwise.
/// File can be both read and written to.
///
/// \param name filename
/// \returns: file
FILE *open_file(char *name)
{
  FILE *file = fopen(name, "r+");

  if (file == NULL)
    {
      // File doesn't exist
      file = fopen(name, "w+");
    }

  return file;
}

/// Closes file
///
/// \param file file to close
void close_file(FILE *file)
{
  fclose(file);
}

/// Stores value of a certain size in file
///
/// \param file file to write to
/// \param value pointer to value
/// \param size size of value
void store(FILE *file, void *value, int size)
{
  fwrite(value, size, 1, file);
}

/// Stores a string to file
///
/// \param file file to store in
/// \param str string to store
void store_string(FILE *file, char *str)
{
  store(file, (void *) str, strlen(str) + 1);
}

/// Stores an int to file
///
/// \param file file to store in
/// \param number int to store
void store_int(FILE *file, int number)
{
  store(file, (void *) &number, sizeof(number));
}

/// Loads a value into a holder
///
/// \param file file to load from
/// \param holder pointer to holder
/// \param size size of value in bytes
void load_value(FILE *file, void *holder, int size)
{
  fread(holder, size, 1, file);
}

/// Loads string from file
///
/// \param file file to load from
/// \param length length of string
/// \returns: pointer to duplicated string
char *load_string(FILE *file, int length)
{
  char buffer[length];
  load_value(file, buffer, length);
  return strdup(buffer);
}

/// Loads int from file
///
/// \param file file to load from
/// \returns: loaded int
int load_int(FILE *file)
{
  int tmp = 0;
  load_value(file, &tmp, sizeof(tmp));
  return tmp;
}
