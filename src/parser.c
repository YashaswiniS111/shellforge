#include <stdio.h>
#include <string.h>

#include "parser.h"

static void print_token_table(Token tokens[], int count)
{
    printf("\n------------ TOKENS ------------\n");

    for (int i = 0; i < count; i++)
    {
        printf(" %d : %-9s %s\n",
               i,
               token_type_name(tokens[i].type),
               tokens[i].value);

        if (tokens[i].type == TOKEN_END)
        {
            break;
        }
    }

    printf("--------------------------------\n");
}

void parse_tokens(Token tokens[], int count)
{
    print_token_table(tokens, count);

    printf("\n========== PIPELINE ==========\n");

    int command_number = 1;
    int start = 0;

    for (int i = 0; i <= count; i++)
    {
        int end_of_command = 0;

        if (i == count)
        {
            end_of_command = 1;
        }
        else if (tokens[i].type == TOKEN_END)
        {
            end_of_command = 1;
        }
        else if (strcmp(tokens[i].value, "|") == 0)
        {
            end_of_command = 1;
        }

        if (!end_of_command)
        {
            continue;
        }

        printf("\nCommand %d\n", command_number);
        printf("----------------------------\n");

        printf("Arguments\n");

        int argument_number = 0;
        const char *input = NULL;
        const char *output = NULL;
        int append = 0;
        int background = 0;

        for (int j = start; j < i; j++)
        {
            if (strcmp(tokens[j].value, "<") == 0)
            {
                if (j + 1 < i)
                {
                    input = tokens[j + 1].value;
                    j++;
                }
            }
            else if (strcmp(tokens[j].value, ">") == 0)
            {
                if (j + 1 < i)
                {
                    output = tokens[j + 1].value;
                    append = 0;
                    j++;
                }
            }
            else if (strcmp(tokens[j].value, ">>") == 0)
            {
                if (j + 1 < i)
                {
                    output = tokens[j + 1].value;
                    append = 1;
                    j++;
                }
            }
            else if (strcmp(tokens[j].value, "&") == 0)
            {
                background = 1;
            }
            else
            {
                printf("argv[%d] = %s\n",
                       argument_number,
                       tokens[j].value);

                argument_number++;
            }
        }

        if (input != NULL)
            printf("Input  : %s\n", input);
        else
            printf("Input  : None\n");

        if (output != NULL)
            printf("Output : %s\n", output);
        else
            printf("Output : None\n");

        printf("Append : %s\n", append ? "Yes" : "No");
        printf("Background : %s\n", background ? "Yes" : "No");

        printf("============================\n");

        command_number++;

        if (i < count &&
            strcmp(tokens[i].value, "|") == 0)
        {
            start = i + 1;
        }
        else
        {
            break;
        }
    }
}
