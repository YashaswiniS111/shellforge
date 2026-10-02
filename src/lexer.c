#include <stdio.h>

#include "lexer.h"

void print_tokens(Token tokens[], int count)
{
    printf("\n------------ TOKENS ------------\n");

    for (int i = 0; i < count; i++)
    {
        printf(" %d : %-9s %s\n",
               i,
               token_type_name(tokens[i].type),
               tokens[i].value);
    }

    printf("--------------------------------\n");
}
