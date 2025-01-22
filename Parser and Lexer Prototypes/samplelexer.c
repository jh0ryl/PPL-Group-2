#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

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

// Token structure
typedef struct
{
    TokenType type;
    char value[50];
} Token;

Token current_token; // Current token

// Global file pointer for logging to Symbol Table
FILE *symbol_table_file;

// Function to get the string representation of a TokenType
const char *token_type_to_string(TokenType type)
{
    switch (type)
    {
    case KEYWORD:
        return "Keyword";
    case RESERVED_WORDS:
        return "Reserved Words";
    case NOISE_WORDS:
        return "Noise Words";
    case IDENTIFIER:
        return "Identifier";
    case NUMBER:
        return "Number";
    case OPERATOR:
        return "Operator";
    case DELIMITER:
        return "Delimiter";
    case COMMENT:
        return "Comment";
    case WHITESPACE:
        return "Whitespace";
    case STRING_LITERAL:
        return "String Literals";
    case ERROR:
        return "Error";
    case END:
        return "End";
    default:
        return "Unknown";
    }
}

// Function to tokenize the input
void next_token(char *input, int *index)
{
    while (isspace(input[*index]))
        (*index)++; // Skip whitespace

    if (strncmp(&input[*index], "int", 3) == 0)
    {
        current_token.type = KEYWORD;
        strcpy(current_token.value, "int");
        *index += 3;
    }
    else if (strncmp(&input[*index], "print", 5) == 0)
    {
        current_token.type = KEYWORD;
        strcpy(current_token.value, "print");
        *index += 5;
    }
    else if (input[*index] == '(')
    {
        current_token.type = DELIMITER;
        strcpy(current_token.value, "(");
        (*index)++;
    }
    else if (input[*index] == ')')
    {
        current_token.type = DELIMITER;
        strcpy(current_token.value, ")");
        (*index)++;
    }
    else if (input[*index] == ',')
    {
        current_token.type = OPERATOR;
        strcpy(current_token.value, ",");
        (*index)++;
    }
    else if (input[*index] == '+')
    {
        current_token.type = OPERATOR;
        strcpy(current_token.value, "+");
        (*index)++;
    }
    else if (input[*index] == '"') // Check for the opening quote of the string literal
    {
        current_token.type = STRING_LITERAL;
        int j = 0;
        current_token.value[j++] = input[*index]; // Start with the opening quote

        (*index)++;                                           // Move past the opening quote
        while (input[*index] != '"' && input[*index] != '\0') // Loop until closing quote or end of string
        {
            // Handle escape sequences inside string literals (e.g., \" or \\)
            if (input[*index] == '\\' && (input[*index + 1] == '"' || input[*index + 1] == '\\'))
            {
                current_token.value[j++] = input[*index]; // Add the backslash
                (*index)++;                               // Move past the backslash
                current_token.value[j++] = input[*index]; // Add the escaped character
            }
            else
            {
                current_token.value[j++] = input[*index]; // Normal character
            }
            (*index)++; // Move to the next character
        }

        if (input[*index] == '"') // If we reach a closing quote
        {
            current_token.value[j++] = input[*index]; // Add the closing quote
            (*index)++;                               // Move past the closing quote
        }

        current_token.value[j] = '\0'; // Null-terminate the string literal
    }

    else if (isalpha(input[*index]))
    {
        int start = *index;
        while (isalnum(input[*index]))
            (*index)++;
        int length = *index - start;
        strncpy(current_token.value, &input[start], length);
        current_token.value[length] = '\0';
        current_token.type = IDENTIFIER;
    }
    else if (input[*index] == '=')
    {
        current_token.type = OPERATOR;
        current_token.value[0] = '=';
        current_token.value[1] = '\0';
        (*index)++;
    }
    else if (isdigit(input[*index]))
    {
        int start = *index;
        while (isdigit(input[*index]))
            (*index)++;
        int length = *index - start;
        strncpy(current_token.value, &input[start], length);
        current_token.value[length] = '\0';
        current_token.type = NUMBER;
    }
    else if (input[*index] == ';')
    {
        current_token.type = DELIMITER;
        current_token.value[0] = ';';
        current_token.value[1] = '\0';
        (*index)++;
    }
    else if (input[*index] == '\0')
    {
        current_token.type = END;
    }
    else
    {
        current_token.type = ERROR;
    }

    // Avoid logging T_END token
    if (current_token.type != END)
    {
        // Log to console and Symbol Table.txt with string representation of TokenType
        printf("%-20s | %-10s\n", current_token.value, token_type_to_string(current_token.type));
        if (symbol_table_file != NULL)
        {
            fprintf(symbol_table_file, "%-20s | %-10s\n", current_token.value, token_type_to_string(current_token.type));
        }
    }
}
