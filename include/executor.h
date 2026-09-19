#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "token.h"

/* Execute one external command or a pipeline */
int execute_pipeline(Token tokens[], int count);

/* Compatibility function for a single external command */
int execute_external(Token tokens[], int count);

#endif
