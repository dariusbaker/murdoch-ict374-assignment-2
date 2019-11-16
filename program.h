#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEPARATOR_CONCURRENT "&"
#define SEPARATOR_INPUT      "<"
#define SEPARATOR_OUTPUT     ">"
#define SEPARATOR_PIPE       "|"
#define SEPARATOR_SEQUENTIAL ";"
#define WILDCARD_ALL         '*'
#define WILDCARD_EXACT       '?'
#define MAX_programS  100
#define MAX_ARGUMENTS 1000

static const char white_space[2] = {(char) 0x20, (char) 0x09};

typedef struct programStruct {
  char * name;
  char * stdin;
  char * stdout;
  char * argv[MAX_ARGUMENTS];
  int    argc;
  int    background;
  int    pipe;
  int    redirection;
} program;

program * make_program(char * input, int background, int pipe);
int    get_arguments(char * input, program * program);
char * get_separator(char * input);
void   print_program(program * program);
void   handle_program_line(
  char * input,
  int background,
  int pipe,
  int iteration,
  program ** programs
);
