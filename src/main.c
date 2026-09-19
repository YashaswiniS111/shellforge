#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "expand.h"
#include "builtin.h"
#include "executor.h"

int main(void)
{
    printf("=====================================\n");
    printf("           Shellforge\n");
    printf("    A Unix Style Shell written in C\n");
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

        Token tokens[MAX_TOKENS];

        int count = tokenize(line, tokens, MAX_TOKENS);

        if (count < 0)
        {
            printf("Tokenization failed.\n");
            free(line);
            continue;
        }

        /*
         * Expand variables such as $HOME.
         */
        expand_tokens(tokens, count);

        /*
         * Display tokens.
         */
        print_tokens(tokens, count);

        /*
         * Display parsed pipeline.
         */
        parse_tokens(tokens, count);

        /*
         * Check whether the command contains a pipeline.
         */
        int has_pipe = 0;

        for (int i = 0; i < count; i++)
        {
            if (tokens[i].type == TOKEN_PIPE)
            {
                has_pipe = 1;
                break;
            }

            if (tokens[i].type == TOKEN_END)
            {
                break;
            }
        }

        /*
         * Built-ins are executed by the shell itself.
         * For this milestone, pipelines are handled by
         * the external command executor.
         */
        if (count > 0 && tokens[0].type != TOKEN_END)
        {
            if (!has_pipe && is_builtin(tokens[0].value))
            {
                if (strcmp(tokens[0].value, "pwd") == 0)
                {
                    builtin_pwd(tokens, count);
                }
                else if (strcmp(tokens[0].value, "cd") == 0)
                {
                    builtin_cd(tokens, count);
                }
                else if (strcmp(tokens[0].value, "echo") == 0)
                {
                    builtin_echo(tokens, count);
                }
                else if (strcmp(tokens[0].value, "exit") == 0)
                {
                    free_tokens(tokens, count);
                    free(line);

                    return 0;
                }
            }
            else
            {
                execute_pipeline(tokens, count);
            }
        }

        free_tokens(tokens, count);
        free(line);
    }

    return 0;
}
