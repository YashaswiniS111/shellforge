#include <stdlib.h>
#include <string.h>

#include "expand.h"

void expand_tokens(Token tokens[], int count)
{
    for (int i = 0; i < count; i++)
    {
        if (tokens[i].type == TOKEN_END)
        {
            break;
        }

        if (tokens[i].value == NULL)
        {
            continue;
        }

        if (tokens[i].value[0] == '$')
        {
            const char *variable = tokens[i].value + 1;
            const char *value = getenv(variable);

            if (value != NULL)
            {
                char *new_value = malloc(strlen(value) + 1);

                if (new_value != NULL)
                {
                    strcpy(new_value, value);

                    free(tokens[i].value);
                    tokens[i].value = new_value;
                }
            }
        }
    }
}
