#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "command.h"
#include "jobs.h"

#define DEBUG 1
#define BUF_SIZE 256
#define BUILTIN_CHANGE_DIR "cd"
#define BUILTIN_EXIT       "exit"
#define BUILTIN_PRINT_DIR  "pwd"
#define BUILTIN_PROMPT     "prompt"

/**
 * Global variables
 */
int next_job_index = 0;
char * shell_name = "%";
Job  * job_list[MAX_COMMANDS];

/**
 * Methods
 */
void collect_children();
void create_piped_processes(Command ** piped_commands, int count);
void create_process(Command * command);
void empty_commands(Command ** commands);
void execute_commands(Command ** commands);
void handle_signals();

int set_redirection(Command * command);
int setup_signals();
int toggle_signal_block(int how, int signal_number);

/**
 * Methods: built-in commands
 */
void change_directory(char * input);
int  print_working_directory();
int  prompt(char * input);

/**
 * Methods: tests
 * These tests help verify that we satisfy project requirements.
 */
void test_background();
void test_combination();
void test_multiple_command_args();
void test_pipes();
void test_redirection();
void test_sequential();