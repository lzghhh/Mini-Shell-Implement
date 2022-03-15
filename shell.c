#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include "token_driver.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

int main(int argc, char **argv) {
  // Weclome message for the shell.
  printf("Welcome to mini-shell.\n");
  char *input = (char *) malloc(sizeof(char) * 256);
  // list for taking the input in seperate parts (like {"ls", "-al"}). 
  char **sorted_list;
  char **prev_command;
  char **temp_sorted_list;

  // The infinite loop for getting user input, break when exit or ctrl + D detected. 
  while (1)
  {
    int status;
    // print the header for the shell command. 
    printf("shell $ ");
    fflush(stdin);
    // use fgets to get input from the user by stdin with 256 characters, if the return of fgets equals to NULL, it means that 
    // ctrl + D is entered by the user and then break, the shell should exit with message as "Bye Bye" in a new line. 
    if (fgets(input, 256, stdin) == NULL) {
      printf("\nBye bye.\n");
      return 0;
    }

    // Set the n + 1 element as NULL for putting into the execvp. 
    input[strlen(input)] = NULL;

    // Use the transfer function in the token_driver.h to transfer a string char * to a string list that seperates different parts 
    // as a char ** type. 
    sorted_list = transfer(input);
    int i = 0;
    while (sorted_list[i] != NULL)
    {
      ++i;
    }
    // Set the n + 1 element as NULL for putting into the execvp.
    sorted_list[i] = NULL;
    temp_sorted_list = sorted_list;

    if (sorted_list[0] == NULL) {
      continue;
    }


    // Compare whether the user input is "exit" or not, if it equals to "exit", then break the while loop and exit the shell with 
    // exiting messages. 
    if (strcmp(sorted_list[0], "exit") == 0) {
      printf("Bye bye.\n");
      return 0;
    }

    // Process for detecting prev
    if (strcmp(sorted_list[0], "prev") == 0) {
      if (prev_command == NULL) {
        printf("No previous command.\n");
        continue;
      } else {
        // use the temp to replace the prev_command avoid double prev override the previous valid one. 
        temp_sorted_list = prev_command;
        sorted_list = prev_command;
        while (sorted_list[i] != NULL) {
          ++i;
        }
      }
    }

    // Process Source
    if (strcmp(sorted_list[0], "source") == 0) {
      if (sorted_list[1] == NULL) {
        printf("No script input.\n");
        prev_command = temp_sorted_list;
        continue;
      } else {
        // make child for source process
        pid_t source_child = fork();
        if (source_child == 0) {
          source_exec(sorted_list[1]);
          exit(0);
        } else {
          waitpid(source_child, NULL, 0);
        }
        // source_exec(sorted_list[1]);
        prev_command = temp_sorted_list;
        continue;
      }
    }

    // Process for cd 
    if (strcmp(sorted_list[0], "cd") == 0) {
      if (sorted_list[1] != NULL) {
        if (chdir(sorted_list[1]) == -1) {
          printf("No such directory called as %s.\n", sorted_list[1]);
        };
      }
      prev_command = temp_sorted_list;
      continue;
    }

    // Print commands for help
    if (strcmp(sorted_list[0], "help") == 0) {
      printf("Mini-Shell HELP - Build in commands: \n");
      printf("cd: cd [Directory Name]                  -- change the current working directory of the shell to the given one.\n");
      printf("source: source [filename]                -- Execute a script.\n");
      printf("prev:                                    -- Prints the previous command line and executes it again.\n");
      printf("help:                                    -- print the helpe menu.\n");
      prev_command = temp_sorted_list;
      continue;
    }

    // Process for finding semicolon
    int semicolon_index[i];
    int semiindex = 0;
    int self_index = 0;
    int semicolon_flag = 0;;
    int semi_count = 0;
    // set up semicolon list index like a ; b ; c, the list is {1, 3, 5}
    while (sorted_list[semiindex] != NULL) {
      if (sorted_list[semiindex][0] == ';') {
        semicolon_index[self_index] = semiindex;
        ++self_index;
        ++semi_count;
        semicolon_flag = 1;
      }
      ++semiindex;
    }

    semicolon_index[self_index] = i;
    // Semicolon appears
    if (semicolon_flag == 1) {
      if (semicolon_index[0] == 0) {
        printf("Invalid position of ;.\n");
        prev_command = temp_sorted_list;
        continue;
       }
      semicolon_recursive(sorted_list, semicolon_index, semi_count);
      prev_command = temp_sorted_list;
      continue;
    }

    // Process pipe process
    int pipe_index[i];
    int pipeindex = 0;
    int pipe_self_index = 0;
    int pipe_flag = 0;;
    int pipe_count = 0;
    // set up pipe list index like a | b | c, the list is {1, 3, 5}
    while (sorted_list[pipeindex] != NULL) {
      if (sorted_list[pipeindex][0] == '|') {
        pipe_index[pipe_self_index] = pipeindex;
        ++pipe_self_index;
        ++pipe_count;
        pipe_flag = 1;
      }
      ++pipeindex;
    }

    pipe_index[pipe_self_index] = i;
    // pipe appears
    if (pipe_flag == 1) {
      if (pipe_index[0] == 0) {
        printf("Invalid position of ;.\n");
        prev_command = temp_sorted_list;
        continue;
       }
      // pipe_recursive(sorted_list, pipe_index, pipe_count, 0, 0);
      int pipe_child = fork();
      if (pipe_child == 0) {
        pipe_recursive(sorted_list, pipe_index, pipe_count, 0, 0);
        exit(0);
      } else {
        int status;
        waitpid(pipe_child, &status, 0);
      }
      
      prev_command = temp_sorted_list;
      continue;
    }

    // No build in type process. 
    main_exec(sorted_list);
    prev_command = temp_sorted_list;
  }
  // free the input char * memory. 
  free(input);


  return 0;
}



int semicolon_recursive(char **input_list, int index_list[], int count) {
  int index_2 = 0;
  // Divide the segment with semicolon with given index. Create a single command
  for (int i = 0; i < count + 1; ++i) {
    int index_1 = 0;
    char **new_list = (char **) malloc(sizeof(char *) * 256); 

    if (i == 0) {
      index_2 = 0;
    } else {
      index_2 = index_list[i - 1] + 1;
    }

    for (int k = index_2; k < index_list[i]; ++k ) {
      new_list[index_1] = malloc(strlen(input_list[k]) + 1);
      strcpy(new_list[index_1], input_list[k]);
      ++index_1;
    }
    new_list[index_1] = NULL;

    // Same pipe process in the main
    int pipe_index[index_1];
    int pipeindex = 0;
    int pipe_self_index = 0;
    int pipe_flag = 0;;
    int pipe_count = 0;
    while (new_list[pipeindex] != NULL) {
      if (new_list[pipeindex][0] == '|') {
        pipe_index[pipe_self_index] = pipeindex;
        ++pipe_self_index;
        ++pipe_count;
        pipe_flag = 1;
      }
      ++pipeindex;
    }

    pipe_index[pipe_self_index] = index_1;
    if (pipe_flag == 1) {
      if (pipe_index[0] == 0) {
        printf("Invalid position of ;.\n");
        continue;
       }
      int pipe_child = fork();
      if (pipe_child == 0) {
        pipe_recursive(new_list, pipe_index, pipe_count, 0, 0);
        exit(0);
      } else {
        int status;
        waitpid(pipe_child, &status, 0);
      }
      continue;
    }
    // Main process for the command
    main_exec(new_list);

    int clean = 0;
    while (new_list[clean] != NULL) {
      free(new_list[clean]);
      ++clean;
    }
    free(new_list);
  }
  return 0;
}


// The process of rediriecting the output. 
int out_redirect(char **input_list, int out_location) {
  char **front_part = (char **) malloc(sizeof(char *) * 256);
  int front_index = 0;
  // Segement the command with two parts, one part before > and one part after > by using input indexs.
  while (front_index < out_location) {
    front_part[front_index] = input_list[front_index];
    ++front_index;
  }

  front_part[front_index] = NULL;
  pid_t child = fork();
  // Entering the child process. 
  if (child == 0) {
    if (close(1) == -1) {
      perror("Error closing stdout");
      exit(1);
    }
    int fd = open(input_list[out_location + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd != 1) {
      perror("Error creating file");
      exit(1);
    }
    // Use execvp to execute the user input with arguments provided from the users in char ** format. 
    // If the return status of execvp equals -1, it means that the command does not execute successfully (the command cannot be found),
    // which will be printed to the stderr to user. 
    if (execvp(front_part[0], front_part) == -1) {
      fprintf(stderr, "No such file or directory\n");
    }
    // should never reach here. 
    return 1;
  }
  free(front_part);
  // Parent will wait for the child to finish after executing its part. 
  waitpid(child, NULL, 0);
}

// The process of rediriecting the input. 
int in_redirect(char **input_list, int in_location) {
  char **front_part = (char **) malloc(sizeof(char *) * 256);
  int front_index = 0;
  // Segement the command with two parts, one part before < and one part after < by using input indexs.
  while (front_index < in_location) {
    front_part[front_index] = input_list[front_index];
    ++front_index;
  }

  front_part[front_index] = NULL;
  pid_t child = fork();
  // Entering the child process. 
  if (child == 0) {
    if (close(0) == -1) {
      perror("Error closing stdin");
      exit(1);
    }
    int fd = open(input_list[in_location + 1], O_RDONLY);
    if (fd != 0) {
      perror("Error reading file");
      exit(1);
    }
    // Use execvp to execute the user input with arguments provided from the users in char ** format. 
    // If the return status of execvp equals -1, it means that the command does not execute successfully (the command cannot be found),
    // which will be printed to the stderr to user. 
    if (execvp(front_part[0], front_part) == -1) {
      fprintf(stderr, "No such file or directory\n");
    }
    // should never reach here. 
    return 1;
  }

  free(front_part);
  // Parent will wait for the child to finish after executing its part. 
  waitpid(child, NULL, 0);
}


// Process the command that has pipe inside. 
int pipe_recursive(char **input_list, int index_list[], int count, int start_index, int loop) {
  char **new_list = (char **) malloc(sizeof(char *) *256);
  int self_index = 0;
  // Get the single command by the given index range. 
  for (int i = start_index; i < index_list[loop]; ++i) {
    new_list[self_index] = (char *) malloc(strlen(input_list[i]) + 1);
    strcpy(new_list[self_index], input_list[i]);
    ++self_index;
  }

  new_list[self_index] = NULL;
  // The situation for the last segement for pipe in the command. 
  if (loop == count) {
    main_exec(new_list);
    int clean = 0;
    while (new_list[clean] != NULL) {
      free(new_list[clean]);
      ++clean;
    }
    free(new_list);
  } else {
    // The first segement or the middle segement that needs the creation of pipes. 
    int pipe_fds[2];
    // Creating the pipe. 
    assert(pipe(pipe_fds) == 0);

    int read_fd = pipe_fds[0];
    int write_fd = pipe_fds[1];
    int child_pid = fork();
    if (child_pid == 0) {
      // Close the read end of pipe. 
      close(read_fd);
      // Close the stdout.
      if (close(1) == -1) {
        perror("Error closing stdout\n");
        exit(1);
      }
      // Push the write side into the next end of pipe. 
      assert(dup(write_fd) == 1);
      close(write_fd);
      main_exec(new_list);
      int clean = 0;
      while (new_list[clean] != NULL) {
        free(new_list[clean]);
        ++clean;
      }
      free(new_list);
      exit(0);
    } else {
      // Main process that allows recursive pipes for the next segement in the pipes. 
      int status;
      // Wait the child to finish. 
      waitpid(child_pid, &status, 0);
      // Close the pipe write end. 
      close(write_fd);
      // Close the stdin. 
      if (close(0) == -1) {
        perror("Error closing stdin");
        exit(1);
      }
      // Push the read side into the next end of pipe. 
      assert(dup(read_fd) == 0);
      close(read_fd);      
      int clean = 0;
      while (new_list[clean] != NULL) {
        free(new_list[clean]);
        ++clean;
      }
      free(new_list);
      int new_location = index_list[loop] + 1;
      // Change the pipe index and start recursion. 
      pipe_recursive(input_list, index_list, count, new_location, ++loop );
     
    
    }
    
  }
  return 0;
}

// Main process for processing commands
int main_exec(char **sorted_list) {
    // Process the output redirect process
    int out_index = 0;
    int out_loop = 0;
    // Find the index of > produce a list of position of >
    while (sorted_list[out_loop] != NULL) {
      if (sorted_list[out_loop][0] == '>') {
        out_index = out_loop;
      }
      ++out_loop;
    }
    // > appears
    if (out_index != 0) {
      out_redirect(sorted_list, out_index);
      return 0;
    }

    // Process the input redirect process
    int in_index = 0;
    int in_loop = 0;
    // Find the index of < produce a list of position of >
    while (sorted_list[in_loop] != NULL) {
      if (sorted_list[in_loop][0] == '<') {
        in_index = in_loop;
      }
      ++in_loop;
    }
    // < appears
    if (in_index != 0) {
      in_redirect(sorted_list, in_index);
      return 0;
    }
    // Create a new process for the user input command that operates in a different process (not the parent process). 
    pid_t child = fork();
    // Entering the child process. 
    if (child == 0) {
      // Use execvp to execute the user input with arguments provided from the users in char ** format. 
      // If the return status of execvp equals -1, it means that the command does not execute successfully (the command cannot be found),
      // which will be printed to the stderr to user. 
      if (execvp(sorted_list[0], sorted_list) == -1) {
        fprintf(stderr, "No such file or directory\n");
      }
      // should never reach here. 
      return 1;
    }
    
    // Parent will wait for the child to finish after executing its part. 
    waitpid(child, NULL, 0);
    return 0;
}


// Process the source command
int source_exec(char *file_name) {
  // open the source file from the command input
  FILE* fd = fopen(file_name, "r");
  if (fd == -1) {
    perror("Failed to open file");
    return 1;
  }
  // Create input space for the each line of the file
  char *input = NULL;
  size_t size = 0;
  // get the line when there is a line.
  while (getline(&input, &size, fd) != -1 ) {
    // The same process of the main.
    char **sorted_list;
    char **prev_command;
    char **temp_sorted_list;
    input[strlen(input)] = NULL;
    sorted_list = transfer(input);
    int i = 0;
    while (sorted_list[i] != NULL)
    {
      ++i;
    }
    // Set the n + 1 element as NULL for putting into the execvp.
    sorted_list[i] = NULL;
    temp_sorted_list = sorted_list;
    if (sorted_list[0] == NULL) {
      continue;
    }


    // Compare whether the user input is "exit" or not, if it equals to "exit", then break the while loop and exit the shell with 
    // exiting messages. 
    if (strcmp(sorted_list[0], "exit") == 0) {
      printf("Bye bye.\n");
      return 0;
    }


    if (strcmp(sorted_list[0], "prev") == 0) {
      if (prev_command == NULL) {
        printf("No previous command.\n");
        continue;
      } else {
        temp_sorted_list = prev_command;
        sorted_list = prev_command;
        while (sorted_list[i] != NULL) {
          ++i;
        }
      }
    }

    if (strcmp(sorted_list[0], "source") == 0) {
      if (sorted_list[1] == NULL) {
        printf("No script input.\n");
        prev_command = temp_sorted_list;
        continue;
      } else {
        source_exec(sorted_list[1]);
        prev_command = temp_sorted_list;
        continue;
      }
    }


    if (strcmp(sorted_list[0], "cd") == 0) {
      if (sorted_list[1] != NULL) {
        if (chdir(sorted_list[1]) == -1) {
          printf("No such directory called as %s.\n", sorted_list[1]);
        };
      }
      prev_command = temp_sorted_list;
      continue;
    }

    if (strcmp(sorted_list[0], "help") == 0) {
      printf("Mini-Shell HELP - Build in commands: \n");
      printf("cd: cd [Directory Name]                  -- change the current working directory of the shell to the given one.\n");
      printf("source: source [filename]                -- Execute a script.\n");
      printf("prev:                                    -- Prints the previous command line and executes it again.\n");
      printf("help:                                    -- print the helpe menu.\n");
      prev_command = temp_sorted_list;
      continue;
    }


    int semicolon_index[i];
    int semiindex = 0;
    int self_index = 0;
    int semicolon_flag = 0;;
    int semi_count = 0;
    while (sorted_list[semiindex] != NULL) {
      if (sorted_list[semiindex][0] == ';') {
        semicolon_index[self_index] = semiindex;
        ++self_index;
        ++semi_count;
        semicolon_flag = 1;
      }
      ++semiindex;
    }

    semicolon_index[self_index] = i;
    if (semicolon_flag == 1) {
      if (semicolon_index[0] == 0) {
        printf("Invalid position of ;.\n");
        prev_command = temp_sorted_list;
        continue;
       }
      semicolon_recursive(sorted_list, semicolon_index, semi_count);
      prev_command = temp_sorted_list;
      continue;
    }


    int pipe_index[i];
    int pipeindex = 0;
    int pipe_self_index = 0;
    int pipe_flag = 0;;
    int pipe_count = 0;
    while (sorted_list[pipeindex] != NULL) {
      if (sorted_list[pipeindex][0] == '|') {
        pipe_index[pipe_self_index] = pipeindex;
        ++pipe_self_index;
        ++pipe_count;
        pipe_flag = 1;
      }
      ++pipeindex;
    }

    pipe_index[pipe_self_index] = i;
    if (pipe_flag == 1) {
      if (pipe_index[0] == 0) {
        printf("Invalid position of ;.\n");
        prev_command = temp_sorted_list;
        continue;
       }
      // pipe_recursive(sorted_list, pipe_index, pipe_count, 0, 0);
      int pipe_child = fork();
      if (pipe_child == 0) {
        pipe_recursive(sorted_list, pipe_index, pipe_count, 0, 0);
        exit(0);
      } else {
        int status;
        waitpid(pipe_child, &status, 0);
      }
      
      prev_command = temp_sorted_list;
      continue;
    }

    main_exec(sorted_list);
    prev_command = temp_sorted_list;
  }
  // Close the file. 
  fclose(fd);
  return 0;
}