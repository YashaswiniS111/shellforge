#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "token.h"

int tokenize(const char *input, Token tokens[], int max_tokens)
{
    int count = 0;
    int i = 0;

    while (input[i] != '\0' && count < max_tokens - 1)
    {
        /* Skip whitespace */
        while (isspace((unsigned char)input[i]))
        {
            i++;
        }

        if (input[i] == '\0')
        {
            break;
        }

        /*
         * Pipeline operator.
         */
        if (input[i] == '|')
        {
            tokens[count].type = TOKEN_PIPE;
            tokens[count].value = malloc(2);

            if (tokens[count].value == NULL)
            {
                perror("malloc");
                free_tokens(tokens, count);
                return -1;
            }

            strcpy(tokens[count].value, "|");

            count++;
            i++;
            continue;
        }

        /*
         * Background operator.
         */
        if (input[i] == '&')
        {
            tokens[count].type = TOKEN_BACKGROUND;
            tokens[count].value = malloc(2);

            if (tokens[count].value == NULL)
            {
                perror("malloc");
                free_tokens(tokens, count);
                return -1;
            }

            strcpy(tokens[count].value, "&");

            count++;
            i++;
            continue;
        }

        /*
         * Read a normal word.
         */
        int start = i;

        while (input[i] != '\0' &&
               !isspace((unsigned char)input[i]) &&
               input[i] != '|' &&
               input[i] != '&')
        {
            i++;
        }

        int length = i - start;

        tokens[count].value = malloc(length + 1);

        if (tokens[count].value == NULL)
        {
            perror("malloc");
            free_tokens(tokens, count);
            return -1;
        }

        strncpy(tokens[count].value, input + start, length);
        tokens[count].value[length] = '\0';

        tokens[count].type = TOKEN_WORD;

        count++;
    }

    /*
     * Add END token.
     */
    if (count < max_tokens)
    {
        tokens[count].type = TOKEN_END;
        tokens[count].value = malloc(4);

        if (tokens[count].value == NULL)
        {
            perror("malloc");
            free_tokens(tokens, count);
            return -1;
        }

        strcpy(tokens[count].value, "END");
        count++;
    }

    return count;
}

void free_tokens(Token tokens[], int count)
{
    for (int i = 0; i < count; i++)
    {
        free(tokens[i].value);
        tokens[i].value = NULL;
    }
}

const char *token_type_name(TokenType type)
{
    switch (type)
    {
        case TOKEN_WORD:
            return "WORD";

        case TOKEN_PIPE:
            return "PIPE";

        case TOKEN_INPUT:
            return "INPUT";

        case TOKEN_OUTPUT:
            return "OUTPUT";

        case TOKEN_APPEND:
            return "APPEND";

        case TOKEN_BACKGROUND:
            return "BACKGROUND";

        case TOKEN_END:
            return "END";

        default:
            return "UNKNOWN";
    }
}
