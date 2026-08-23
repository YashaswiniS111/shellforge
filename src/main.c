#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "token.h"
#include "parser.h"
#include "expand.h"

int main(void)
{
    printf("=====================================\n");
    printf("           SHELLFORGE\n");
    printf("       Milestone 2.2 - Parser\n");
    printf("=====================================\n");

    while (1)
    {
        char *line = readline("shellforge$ ");

        if (line == NULL)
        {
            printf("\nExiting Shellforge...\n");
            break;
        }

        if (strlen(line) == 0)
        {
            free(line);
            continue;
        }

        add_history(line);

        if (strcmp(line, "exit") == 0)
        {
            free(line);
            printf("Exiting Shellforge...\n");
            break;
        }

        Token tokens[MAX_TOKENS];

        int count = tokenize(line, tokens, MAX_TOKENS);

        if (count < 0)
        {
            printf("Tokenization failed.\n");
            free(line);
            continue;
        }

        expand_tokens(tokens, count);

        parse_tokens(tokens, count);

        free_tokens(tokens, count);
        free(line);
    }

    return 0;
}
