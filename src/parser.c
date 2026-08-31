#include <stdio.h>

#include "parser.h"

void parse_tokens(Token tokens[], int count)
{
    printf("\n========== PIPELINE ==========\n\n");

    printf("Command 1\n");
    printf("------------------------------\n");

    printf("Arguments\n");

    int argument = 0;

    for (int i = 0; i < count; i++)
    {
        if (tokens[i].type == TOKEN_END)
            break;

        printf("argv[%d] = %s\n", argument, tokens[i].value);

        argument++;
    }

    printf("Input      : None\n");
    printf("Output     : None\n");
    printf("Append     : No\n");
    printf("Background : No\n");

    printf("==============================\n");
}
