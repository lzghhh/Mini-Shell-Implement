#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include "token_driver.h"

// Determine whether the input character is a digit. 
int is_digit(char input) {
  return input >= '0' && input <= '9';
}

// Determine whether the input character is a alphabetic letter.(not used yet).
int is_alpha(char input) {
  return (('z' >= input && 'a' <= input) || ('Z' >= input && 'A' <= input));
}


// Read the next integer as a string from the input into the output.
int read_integer_string(const char *input, char *output) {
  int i = 0;
  // while we have input and the character is a digit,
  while (input[i] != '\0' && is_digit(input[i])) {
    output[i] = input[i]; // copy character to output list.
    ++i;
  }
  output[i] = '\0'; // add the terminating byte for the string input into the char **. 

  return i; // return the input slot string length that we should move i spaces in the input user string. 
}

// Read the input slot information into char * in a char ** return the length of the char *.
int read_str(const char *input, char *output) {
  int i = 0;
  // while we have input and the character is a string,
  while (input[i] != '\0' && input[i] != ' ' && input[i] != '\n' && input[i] != '(' && input[i] != ')' && input[i] != '>'
  && input[i] != '<' && input[i] != ';' && input[i] != '|') {
    output[i] = input[i]; // copy character to output list.
    ++i;
  }
  output[i] = '\0'; // add the terminating byte for the string input into the char **. 

  return i; // return the input slot string length that we should move i spaces in the input user string. 
}

// Read the information inside a quotation mark. 
int read_in_quotation(const char *input, char *output) {
  int i = 0;
  // while we have input and it is still inside quotation mark,
  while (input[i] != '"') {
    output[i] = input[i]; // copy character to output list.
    ++i;
  }
  output[i] = '\0'; // add the terminating byte for the string input into the char **. 

  return i;
}

// Transfer a char * string into a char ** array with separated slots for "", normal string and special characters. 
char **transfer(char *argv) {
  // Allocate the space for the list of separated string slots from the user's single string input. 
  char **sorted_list;
  sorted_list = (char **) malloc(sizeof(char *) * 256);
  // Buffer string used for putting the string into sorted list. 
  char buf[256];

  // i stands for the index of the user input string char *. 
  int i = 0;
  // x stands for the index of the index of the sort_list (separated list) current position. 
  int x = 0;

  // For the char * in the user input not equals to the new line symbol or the string ending \0 character, continue read the input character. 
  while (argv[i] != '\0' && argv[i] != '\n') {
    
    // Situation 2: The input is in a quotation mark. 
    if (argv[i] == '"') {
      // Move the i index in the user input string to next slot. 
      i += read_in_quotation(&argv[i + 1], buf) + 2;
      // Allocate the char * space for the user's input string in this slot. 
      sorted_list[x] = (char *) malloc(strlen(buf) + 1);
      // Copy the buffer string into the separated list with given i index position.
      strcpy(sorted_list[x], buf); 
      ++x; // Move to the next char ** index. 
      continue; // Continue to the next while loop without executing the rest code below.
    }

    // Temporary holder the special character. 
    char *holder = (char *) malloc(sizeof(char));

    // Searching for any special character or default setting (normal word input). 
    switch (argv[i]) {
      case '(':
        holder[0] = (char) 40; // Set the holder to character with the int representation of the character. 
        sorted_list[x] = holder; // Put the special character in a single array index in the separated list. 
        ++x; // Move to the next char ** index. 
        break;
      case ')' :
        holder[0] = (char) 41; 
        sorted_list[x] = holder; 
        ++x; 
        break;
      case '<' :
        holder[0] = (char) 60;  
        sorted_list[x] = holder;
        ++x;  
        break;
      case '>' :
        holder[0] = (char) 62; 
        sorted_list[x] = holder;
        ++x; 
        break;
      case ';':
        holder[0] = (char) 59; 
        sorted_list[x] = holder;
        ++x; 
        break;
      case '|' :
        holder[0] = (char) 124;  
        sorted_list[x] = holder;
        ++x; 
        break;
      case ' ': // Skip the space between the user input. 
        break;
      // Default setting as the user puts a normal word, and the read_str will just read every character until the next space regardless
      // of special characters. 
      default:
        i += read_str(&argv[i], buf) - 1; // -1 stands for compensating the ++i later in LIne 146.  
        sorted_list[x] = (char *) malloc(strlen(buf) + 1);
        strcpy(sorted_list[x], buf); 
        ++x; // Move to the next char ** index. 
        break;
    }

    ++i; // Move the user input by one space since it is a single special character. 
  }
  // Return the finalized separated list. 
  return sorted_list;
}

