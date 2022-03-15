#ifndef _TOKEN_DRIVER_H
#define _TOKEN_DRIVER_H

// Determine whether the input character is a digit. 
int is_digit(char input);

// Determine whether the input character is a alphabetic letter.(not used yet).
int is_alpha(char input);

// Read the next integer as a string from the input into the output.
int read_integer_string(const char *input, char *output);

// Read the input slot information into char * in a char ** return the length of the char *
int read_str(const char *input, char *output);

// Read the information inside a quotation mark. 
int read_in_quotation(const char *input, char *output);

// Transfer a char * string into a char ** array with separated slots for "", normal string and special characters. 
char **transfer(char *argv);

#endif /* ifndef _TOKEN_DRIVER_H */
