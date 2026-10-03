#ifndef BUILTIN_H
#define BUILTIN_H

#include "token.h"

/* Built-in command functions */
int builtin_pwd(Token tokens[], int count);
int builtin_cd(Token tokens[], int count);
int builtin_echo(Token tokens[], int count);
int builtin_exit(Token tokens[], int count);

int builtin_jobs(Token tokens[], int count);
int builtin_fg(Token tokens[], int count);
int builtin_bg(Token tokens[], int count);

/* Execute a built-in command */
int execute_builtin(Token tokens[], int count);

/* Check whether a command is a built-in */
int is_builtin(const char *command);

#endif
