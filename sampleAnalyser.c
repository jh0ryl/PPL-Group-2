#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_SIZE 100

// Define possible token types
typedef enum
{
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    OPERATOR,
    DELIMITER,
    COMMENT,
    ERROR
} TokenType;

// Keywords list
const char *keywords[] = {"int", "float", "if", "else", "return", "while"};
int isKeyword(const char *str)
{
    for (int i = 0; i < 6; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// Token structure
typedef struct
{
    TokenType type;
    char value[MAX_TOKEN_SIZE];
} Token;

// Function to classify and print token
void printToken(Token token)
{
    switch (token.type)
    {
    case KEYWORD:
        printf("Keyword: %s\n", token.value);
        break;
    case IDENTIFIER:
        printf("Identifier: %s\n", token.value);
        break;
    case NUMBER:
        printf("Number: %s\n", token.value);
        break;
    case OPERATOR:
        printf("Operator: %s\n", token.value);
        break;
    case DELIMITER:
        printf("Delimiter: %s\n", token.value);
        break;
    case COMMENT: // Handle comment case
        printf("Comment: %s\n", token.value);
        break;
    default:
        printf("Error: Unknown token %s\n", token.value);
    }
}

// Lexical analyzer function
void lexicalAnalyzer(const char *input)
{
    int i = 0, j = 0;
    char currentChar;
    Token currentToken;

    while ((currentChar = input[i]) != '\0')
    {
        // Skip whitespace
        if (isspace(currentChar))
        {
            i++;
            continue;
        }

        // Handle single-line comments (starting with #)
        if (currentChar == '#')
        {
            currentToken.type = COMMENT;
            j = 0;
            currentToken.value[j++] = currentChar; // Store the '#' character

            // Collect all characters until the end of the line
            i++;
            while (input[i] != '\n' && input[i] != '\0')
            {
                currentToken.value[j++] = input[i++];
            }
            currentToken.value[j] = '\0';
            printToken(currentToken);
        }
        // Handle multi-line comments (starting with """ and ending with """)
        else if (input[i] == '"' && input[i + 1] == '"' && input[i + 2] == '"')
        {
            currentToken.type = COMMENT;
            j = 0;
            currentToken.value[j++] = input[i++];
            currentToken.value[j++] = input[i++];
            currentToken.value[j++] = input[i++];

            // Collect all characters until the closing triple quotes
            while (!(input[i] == '"' && input[i + 1] == '"' && input[i + 2] == '"') && input[i] != '\0')
            {
                currentToken.value[j++] = input[i++];
            }

            // Add closing triple quotes if found
            if (input[i] == '"' && input[i + 1] == '"' && input[i + 2] == '"')
            {
                currentToken.value[j++] = input[i++];
                currentToken.value[j++] = input[i++];
                currentToken.value[j++] = input[i++];
            }

            currentToken.value[j] = '\0';
            printToken(currentToken);
        }
        
        // Handle identifiers and keywords
        else if (isalpha(currentChar))
        {
            j = 0;
            while (isalnum(input[i]) || input[i] == '_')
            {
                currentToken.value[j++] = input[i++];
            }
            currentToken.value[j] = '\0';
            currentToken.type = isKeyword(currentToken.value) ? KEYWORD : IDENTIFIER;
            printToken(currentToken);
        }
               // Handle multi-line comments (starting with "" and ending with "")
        else if (input[i] == '"' && input[i + 1] == '"')
        {
            currentToken.type = COMMENT;
            j = 0;
            currentToken.value[j++] = input[i++];
            currentToken.value[j++] = input[i++];

            // Collect all characters until the closing double quotes
            while (!(input[i] == '"' && input[i + 1] == '"') && input[i] != '\0')
            {
                currentToken.value[j++] = input[i++];
            }

            // Add closing double quotes if found
            if (input[i] == '"' && input[i + 1] == '"')
            {
                currentToken.value[j++] = input[i++];
                currentToken.value[j++] = input[i++];
            }

            currentToken.value[j] = '\0';
            printToken(currentToken);
        }
        // Handle numbers
        else if (isdigit(currentChar))
        {
            j = 0;
            while (isdigit(input[i]))
            {
                currentToken.value[j++] = input[i++];
            }
            currentToken.value[j] = '\0';
            currentToken.type = NUMBER;
            printToken(currentToken);
        }
        // Handle operators
        else if (strchr("+-*/=<>", currentChar))
        {
            currentToken.value[0] = currentChar;
            currentToken.value[1] = '\0';
            currentToken.type = OPERATOR;
            printToken(currentToken);
            i++;
        }
        // Handle delimiters
        else if (strchr(";,(){}", currentChar))
        {
            currentToken.value[0] = currentChar;
            currentToken.value[1] = '\0';
            currentToken.type = DELIMITER;
            printToken(currentToken);
            i++;
        }
        // Handle unknown characters
        else
        {
            currentToken.value[0] = currentChar;
            currentToken.value[1] = '\0';
            currentToken.type = ERROR;
            printToken(currentToken);
            i++;
        }
    }
}

int main()
{
    char input[1000];

    // Ask the user for input
    printf("Enter a line of code for lexical analysis:\n");
    fgets(input, sizeof(input), stdin); // Read user input

    printf("\nTokens:\n");
    lexicalAnalyzer(input);
    return 0;
}
