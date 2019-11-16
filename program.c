#include "program.h"

/**
 * Helper program that prints program attributes.
 * @param {program *} program
 */
void print_program(program * program) {
  printf("-----------------------------------------------\n");
  printf("| %-24s | %-16s |\n", "Name", program->name);
  printf("| %-24s | %-16d |\n", "argc", program->argc);
  for (int i = 0; i < program->argc; i++) {
    printf("| argv[%d]                  | %-16s |\n", i, program->argv[i]);
  }
  printf("| %-24s | %-16d |\n", "Runs in background?", program->background);
  printf("| %-24s | %-16d |\n", "Pipes to program",    program->pipe);
  printf("| %-24s | %-16d |\n", "Redirection",         program->redirection);
  printf("| %-24s | %-16s |\n", "Redirect input",      program->stdin);
  printf("| %-24s | %-16s |\n", "Redirect output",     program->stdout);
  printf("-----------------------------------------------\n");
}

/**
 * Splits a string by whitespace to extract arguments (argv) and saves them
 * into the program instance provided.
 * @param  {char *}    input
 * @param  {program *} program
 * @return {int}
 */
int get_arguments(char * input, program * program) {
  char * token = NULL;
  int argument_count = 0;

  // split input by whitespace to derive arguments
  token = strtok(input, white_space);

  while (token != NULL) {
    /**
     * Searches for all pathnames matching pattern.
     * Satisfies:
     * - Issue #9
     * - Requirement #6
     * - Marking guide #5
     */
    if (index(token, WILDCARD_ALL) != NULL || index(token, WILDCARD_EXACT) != NULL) {
      glob_t result;

      glob(token, 0, NULL, &result);

      // add expanded patterns to argv
      for (int i = 0; i < result.gl_pathc; i++) {
        program->argv[argument_count] = strdup(result.gl_pathv[i]);
        argument_count++;
      }

      globfree(&result);
    } else {
      // duplicate string and add to arguments array
      program->argv[argument_count] = strdup(token);

      // increment count
      argument_count++;
    }

    // get the next argument
    token = strtok(NULL, white_space);
  }

  return argument_count;
}

/**
 * Searches for a separator (;&|) and returns the character if found.
 * @param  {char *} input
 * @return {char *}
 */
char * get_separator(char * input) {
  char * separator = NULL;

  if ((separator = index(input, * SEPARATOR_SEQUENTIAL)) != NULL) {
    // we found a ';' character
    separator = SEPARATOR_SEQUENTIAL;
  } else if ((separator = index(input, * SEPARATOR_CONCURRENT)) != NULL) {
    // we found a '&' character
    return SEPARATOR_CONCURRENT;
  } else if ((separator = index(input, * SEPARATOR_PIPE)) != NULL) {
    // we found a '|' character
    return SEPARATOR_PIPE;
  }

  return separator;
}

/**
 * Populates a program struct with the found/required attributes and returns
 * a pointer to that instance.
 * @param  {char *}    input
 * @param  {int}       background
 * @param  {int}       pipe
 * @return {program *}
 */
program * make_program(char * input, int background, int pipe) {
  char    * token         = NULL;
  char    * stdin_result  = index(input, * SEPARATOR_INPUT);
  char    * stdout_result = index(input, * SEPARATOR_OUTPUT);
  program * program       = calloc(1, sizeof(program));

  if (stdout_result != NULL) {
    // output redirection found
    token = strtok(input, SEPARATOR_OUTPUT);

    // get output destination
    token = strtok(NULL, " ");

    // save the redirection; 1 for standard input (fd[1])
    program->redirection = 1;
    program->stdout = strdup(token);
  } else if (stdin_result != NULL) {
    // input redirection found
    token = strtok(input, SEPARATOR_INPUT);

    // get input destination
    token = strtok(NULL, " ");

    // save the redirection; 0 for standard input (fd[0])
    program->redirection = 0;
    program->stdin = strdup(token);
  }

  // save other attributes
  program->argc       = get_arguments(input, program);
  program->name       = program->argv[0];
  program->background = background;
  program->pipe       = pipe;

  //print_program(program);

  // only return the struct if there're valid arguments
  if (program->argc > 0) {
    return program;
  } else {
    free(program);
    return NULL;
  }
}

/**
 * Processes a program line entry, extracts jobs and programs, and then saves
 * these programs into an array containing program structs.
 * @param {char *}     input
 * @param {int}        background
 * @param {int}        pipe
 * @param {program **} programs
 */
void handle_program_line(
  char * input,
  int background,
  int pipe,
  int iteration,
  program ** programs
) {
  int is_background = 0;
  int is_pipe       = 0;
  char * separator  = NULL;
  char * token      = NULL;
  static int program_count;

  // reset program count if we're running this method as a fresh run
  if (iteration == 0) {
    program_count = 0;
  }

  if ((separator = get_separator(input)) != NULL) {
    // separators found
    token = strtok(input, separator);
    token = strtok(NULL, "");

    if (strcmp(separator, SEPARATOR_CONCURRENT) == 0) {
      is_background = 1;
    }

    if (strcmp(separator, SEPARATOR_PIPE) == 0) {
      is_pipe = program_count + 1;
    }

    // process the input again to see if anymore separators are present
    handle_program_line(input, is_background, is_pipe, 1, programs);
  } else {
    // no special characters found
    programs[program_count] = make_program(input, background, pipe);
    program_count++;
  }

  // run method again if input is still available
  if (token != NULL) {
    handle_program_line(token, background, pipe, 1, programs);
  }
}
