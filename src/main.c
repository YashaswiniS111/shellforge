#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "token.h"
#include "lexer.h"

int main(void)
{
    char *line;

    /* Milestone 1 welcome banner */
    printf("=====================================\n");
    printf("           Shellforge\n");
    printf(" A Unix Style Shell written in C\n");
    printf("=====================================\n");

    while (1)
    {
        line = readline("shellforge$ ");

        /* Ctrl+D */
        if (line == NULL)
        {
            printf("\nGoodbye!\n");
            break;
        }

        /* Ignore empty input */
        if (strlen(line) == 0)
        {
            free(line);
            continue;
        }

        add_history(line);

        /* Exit command */
        if (strcmp(line, "exit") == 0)
        {
            free(line);
            printf("Exiting Shellforge...\n");
            break;
        }

        /* Tokenize input */
        Token tokens[MAX_TOKENS];

        int count = tokenize(line, tokens, MAX_TOKENS);

        if (count < 0)
        {
            free(line);
            continue;
        }

        /* Display tokens */
        print_tokens(tokens, count);

        /* Free token memory */
        free_tokens(tokens, count);

        free(line);
    }

    return 0;
}
