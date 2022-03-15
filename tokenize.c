#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include "token_driver.h"

int main(int argc, char **argv) {
  // Allocate memory space for the input from the user. 
  char *input = (char *) malloc(sizeof(char) * 256);
  // Use fgets to get user's input from stdin with maximum of 256 characters.  
  fgets(input, 256, stdin);

  // List for separated user's inputs frm the input. 
  char **sorted_list;
  // Allocate space for the separated list that is processed. 
  sorted_list = (char **) malloc(sizeof(char *) * 256);

  // TODO: Implement the tokenize demo.
  // Transfer the char * string input to the char ** separated lists with function transfer in token_driver.h. 
  sorted_list = transfer(input);
  // Print each element char * in the char ** separated list and free their allocated memory. 
  int i = 0;
  while (sorted_list[i] != NULL)
  {
    printf("%s\n", sorted_list[i]);
    free(sorted_list[i]);
    ++i;
  }

  // Free the separated list memory. 
  free(sorted_list);
}
