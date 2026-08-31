#ifndef TOKEN_H
#define TOKEN_H

#define MAX_TOKENS 100

typedef enum
{
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_INPUT,
    TOKEN_OUTPUT,
    TOKEN_APPEND,
    TOKEN_BACKGROUND,
    TOKEN_END
} TokenType;

typedef struct
{
    TokenType type;
    char *value;
} Token;

int tokenize(const char *input, Token tokens[], int max_tokens);

void free_tokens(Token tokens[], int count);

const char *token_type_name(TokenType type);

#endif
