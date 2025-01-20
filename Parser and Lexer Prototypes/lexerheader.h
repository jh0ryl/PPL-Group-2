#ifndef LEXER_H
#define LEXER_H

// Token Categories
typedef enum
{
    KEYWORD,
    RESERVED_WORDS,
    NOISE_WORDS,
    IDENTIFIER,
    NUMBER,
    OPERATOR,
    DELIMITER,
    COMMENT,
    WHITESPACE,
    STRING_LITERAL,
    ERROR,
    END
} TokenType;

typedef struct
{
    TokenType type;
    char value[50];
} Token;

extern Token current_token;

void next_token(char *input, int *index);

#endif // LEXER_H
