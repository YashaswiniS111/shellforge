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

        int start = i;

        /*
         * Read one word.
         * For Milestone 2, spaces separate tokens.
         */
        while (input[i] != '\0' &&
               !isspace((unsigned char)input[i]))
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

    /* Add END token */
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

        case TOKEN_END:
            return "END";

        default:
            return "UNKNOWN";
    }
}
