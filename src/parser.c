#include <stdio.h>

#include "parser.h"

void parse_tokens(Token tokens[], int count)
{
    printf("\n========== PIPELINE ==========\n\n");

    int command_number = 1;
    int argument = 0;

    int background = 0;

    printf("Command %d\n", command_number);
    printf("------------------------------\n");
    printf("Arguments\n");

    for (int i = 0; i < count; i++)
    {
        if (tokens[i].type == TOKEN_END)
        {
            break;
        }

        if (tokens[i].type == TOKEN_BACKGROUND)
        {
            background = 1;
            continue;
        }

        if (tokens[i].type == TOKEN_PIPE)
        {
            printf("Input      : None\n");
            printf("Output     : Pipe\n");
            printf("Append     : No\n");
            printf("Background : %s\n",
                   background ? "Yes" : "No");

            printf("==============================\n\n");

            command_number++;
            argument = 0;

            printf("Command %d\n", command_number);
            printf("------------------------------\n");
            printf("Arguments\n");

            continue;
        }

        printf("argv[%d] = %s\n", argument, tokens[i].value);
        argument++;
    }

    printf("Input      : None\n");
    printf("Output     : None\n");
    printf("Append     : No\n");
    printf("Background : %s\n",
           background ? "Yes" : "No");

    printf("==============================\n");
}
