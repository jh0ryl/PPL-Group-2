#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_SIZE 1000

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
    STRING_LITERALS,
    CHARACTER,
    ERROR,
    DATA_TYPE
} TokenType;

// Token structure
typedef struct
{
    TokenType type;
    char value[MAX_TOKEN_SIZE];
    size_t line_number;
} Token;

size_t line_number = 1;
// Function to classify and print token
void printToken(Token token, FILE *file)
{
    printf("Line Number: %zu ", line_number);
    // Print to console
    switch (token.type)
    {
    case KEYWORD:
        printf("Keyword: %s\n", token.value);
        break;
    case RESERVED_WORDS:
        printf("Reserved Word: %s\n", token.value);
        break;
    case NOISE_WORDS:
        printf("Noise Word: %s\n", token.value);
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
    case COMMENT:
        printf("Comment: %s\n", token.value);
        break;
    case WHITESPACE:
        printf("Whitespace: %s\n", token.value);
        break;
    case STRING_LITERALS:
        printf("String Literal: %s\n", token.value);
        break;
    case CHARACTER:
        printf("Character: %s\n", token.value);
        break;
    case DATA_TYPE:
        printf("Data Type: %s\n", token.value);
        break;
    default:
        printf("Error: Unknown token %s\n", token.value);
        break;
    }

    // Write to the file
    const char *tokenTypeStr;
    switch (token.type)
    {
    case KEYWORD:
        tokenTypeStr = "KEYWORD";
        break;
    case RESERVED_WORDS:
        tokenTypeStr = "RESERVED_WORD";
        break;
    case NOISE_WORDS:
        tokenTypeStr = "NOISE_WORD";
        break;
    case IDENTIFIER:
        tokenTypeStr = "IDENTIFIER";
        break;
    case NUMBER:
        tokenTypeStr = "NUMBER";
        break;
    case OPERATOR:
        tokenTypeStr = "OPERATOR";
        break;
    case DELIMITER:
        tokenTypeStr = "DELIMITER";
        break;
    case COMMENT:
        tokenTypeStr = "COMMENT";
        break;
    case WHITESPACE:
        tokenTypeStr = "WHITESPACE";
        break;
    case STRING_LITERALS:
        tokenTypeStr = "STRING_LITERAL";
        break;
    case CHARACTER:
        tokenTypeStr = "CHARACTER";
        break;
    case DATA_TYPE:
        tokenTypeStr = "DATA_TYPE";
        break;
    default:
        tokenTypeStr = "ERROR";
    }

    fprintf(file, "%-20s %-15s %-10zu\n", token.value, tokenTypeStr, token.line_number);
}

// Lexical analyzer function
void lexicalAnalyzer(const char *input, FILE *file)
{
    int error = 0;
    int i = 0, j = 0;
    char currentChar;
    Token currentToken;

    while ((currentChar = input[i]) != '\0')
    {
        //  Actual Whitespace
        if (isspace(currentChar))
        {
            if (currentChar == '\n')
            {
                line_number++;
            }
            i++;
            continue;
        }

        // Check for \n
        if (input[i] == '\\' && input[i + 1] == 'n')
        {
            currentToken.value[0] = '\\';
            currentToken.value[1] = 'n';
            currentToken.value[2] = '\0';
            currentToken.type = WHITESPACE;
            currentToken.line_number = line_number;
            printToken(currentToken, file);
            i += 2;
        }

        //  Single-line comments
        if (currentChar == '#')
        {
            currentToken.type = COMMENT;
            currentToken.line_number = line_number;
            j = 0;
            currentToken.value[j++] = currentChar;

            i++;
            while (input[i] != '\n' && input[i] != '\0')
            {
                currentToken.value[j++] = input[i++];
            }
            currentToken.value[j] = '\0';
            printToken(currentToken, file);
        }

        // Multi-line comments
        else if (input[i] == '"' && input[i + 1] == '"' && input[i + 2] == '"')
        {
            currentToken.type = COMMENT;
            currentToken.line_number = line_number;
            j = 0;
            currentToken.value[j++] = input[i++];
            currentToken.value[j++] = input[i++];
            currentToken.value[j++] = input[i++];

            while (!(input[i] == '"' && input[i + 1] == '"' && input[i + 2] == '"') && input[i] != '\0')
            {
                currentToken.value[j++] = input[i++];
            }

            if (input[i] == '"' && input[i + 1] == '"' && input[i + 2] == '"')
            {
                currentToken.value[j++] = input[i++];
                currentToken.value[j++] = input[i++];
                currentToken.value[j++] = input[i++];
            }

            currentToken.value[j] = '\0';
            printToken(currentToken, file);
        }

        // Handle string literals (quoted strings)
        else if (currentChar == '"')
        {
            currentToken.type = STRING_LITERALS;
            currentToken.line_number = line_number;
            j = 0;
            currentToken.value[j++] = currentChar;

            i++;
            while (input[i] != '"' && input[i] != '\0')
            {
                // Handle escape sequences inside string literals
                if (input[i] == '\\' && (input[i + 1] == '"' || input[i + 1] == '\\'))
                {
                    currentToken.value[j++] = input[i++];
                    currentToken.value[j++] = input[i++];
                }
                else
                {
                    currentToken.value[j++] = input[i++];
                }
            }

            if (input[i] == '"')
            {
                currentToken.value[j++] = input[i++];
            }

            currentToken.value[j] = '\0';
            printToken(currentToken, file);
        }

        // Handle single-quoted character literals
        else if (currentChar == '\'')
        {
            currentToken.type = CHARACTER;
            currentToken.line_number = line_number;
            j = 0;

            currentToken.value[j++] = currentChar; // Add the opening quote
            i++;

            int charCount = 0; // Count valid characters inside the single quotes

            // Process characters inside the single quotes
            while (input[i] != '\'' && input[i] != '\0') // Stop at closing quote or end of input
            {
                if (charCount >= 1) // If there's more than one character, mark it as an error
                {
                    currentToken.type = ERROR;
                }

                // Add character to the token value
                currentToken.value[j++] = input[i++];
                charCount++;
            }

            // Check for the closing quote
            if (input[i] == '\'')
            {
                currentToken.value[j++] = input[i++]; // Add the closing quote
            }
            else
            {
                // If no closing quote, it's an error
                currentToken.type = ERROR;
            }

            currentToken.value[j] = '\0';

            // Final check: If it's not exactly one character, set as an error
            if (charCount != 1)
            {
                currentToken.type = ERROR;
            }

            // Print the token
            printToken(currentToken, file);
        }

        // Handle invalid characters like special characters
        if (strchr("@#.`?_", input[i])) // If invalid character is found
        {
            // Mark the entire token as an error
            currentToken.type = ERROR;
            int j = 0;                                                                           // Start filling the token value
            while (!isspace(input[i]) && input[i] != '\0' && !strchr("{};,()[].'\"'", input[i])) // Process until space or delimiter
            {
                currentToken.value[j++] = input[i++];
            }
            currentToken.value[j] = '\0';           // Null-terminate the token value
            currentToken.line_number = line_number; // Set line number
            printToken(currentToken, file);         // Print the token as an error
        }

        // Handle identifiers, reserved words, keywords, and noise
        else if (isalpha(currentChar))
        {

            j = 0;
            int start_index = 0, error = 0;
            while (isalnum(input[i]) || input[i] == '_')
            {
                currentToken.value[j++] = input[i++];
            }
            currentToken.value[j] = '\0';
            if (error)
            {
                currentToken.type = ERROR;
                currentToken.line_number = line_number;
                error = 0;
            }
            else
            {
                switch (currentToken.value[start_index])
                {
                case 'i': // words that start with 'i'
                    switch (currentToken.value[start_index + 1])
                    {
                    case 'f':
                        if (currentToken.value[start_index + 2] == '\0')
                        {
                            currentToken.type = KEYWORD; // if
                            currentToken.line_number = line_number;
                        }
                        else
                        {
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                        }
                        break;
                    case 'n':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 't':
                            if (currentToken.value[start_index + 3] == '\0')
                            {
                                currentToken.type = DATA_TYPE; // int
                                currentToken.line_number = line_number;
                            }
                            else
                            {
                                currentToken.type = IDENTIFIER;
                                currentToken.line_number = line_number;
                            }
                            break;
                        case 'p':
                            switch (currentToken.value[start_index + 3])
                            {
                            case 'u':
                                switch (currentToken.value[start_index + 4])
                                {
                                case 't':
                                    if (currentToken.value[start_index + 5] == '\0')
                                    {
                                        currentToken.type = KEYWORD; // input
                                        currentToken.line_number = line_number;
                                    }
                                    else
                                    {
                                        currentToken.type = IDENTIFIER;
                                        currentToken.line_number = line_number;
                                    }
                                    break;
                                default:
                                    currentToken.type = IDENTIFIER;
                                    currentToken.line_number = line_number;
                                }
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                currentToken.line_number = line_number;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                        }
                        break;
                    default:
                        currentToken.type = IDENTIFIER;
                        currentToken.line_number = line_number;
                    }
                    break;
                case 'a': // words that start with 'a'
                    switch (currentToken.value[start_index + 1])
                    {
                    case 'u':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 't':
                            switch (currentToken.value[start_index + 3])
                            {
                            case 'o':
                                if (currentToken.value[start_index + 4] == '\0')
                                {
                                    currentToken.type = NOISE_WORDS; // auto
                                    currentToken.line_number = line_number;
                                }
                                else
                                {
                                    currentToken.type = IDENTIFIER;
                                    currentToken.line_number = line_number;
                                }
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                currentToken.line_number = line_number;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                        }
                        break;
                    default:
                        currentToken.type = IDENTIFIER;
                        currentToken.line_number = line_number;
                    }
                    break;
                case 'b': // words that start with 'b'
                    switch (currentToken.value[start_index + 1])
                    {
                    case 'o':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 'o':
                            switch (currentToken.value[start_index + 3])
                            {
                            case 'l':
                                if (currentToken.value[start_index + 4] == '\0')
                                {
                                    currentToken.type = DATA_TYPE; // bool
                                    currentToken.line_number = line_number;
                                }
                                else
                                {
                                    currentToken.type = IDENTIFIER;
                                    currentToken.line_number = line_number;
                                }
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                currentToken.line_number = line_number;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                        }
                        break;
                    case 'r':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 'e':
                            switch (currentToken.value[start_index + 3])
                            {
                            case 'a':
                                switch (currentToken.value[start_index + 4])
                                {
                                case 'k':
                                    if (currentToken.value[start_index + 5] == '\0')
                                    {
                                        currentToken.type = KEYWORD; // break
                                        currentToken.line_number = line_number;
                                    }
                                    else
                                    {
                                        currentToken.type = IDENTIFIER;
                                        currentToken.line_number = line_number;
                                    }
                                    break;
                                default:
                                    currentToken.type = IDENTIFIER;
                                    currentToken.line_number = line_number;
                                }
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                currentToken.line_number = line_number;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                        }
                        break;
                    default:
                        currentToken.type = IDENTIFIER;
                        currentToken.line_number = line_number;
                    }
                    break;
                case 'c':
                    switch (currentToken.value[start_index + 1])
                    {
                    case 'h':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 'a':
                            switch (currentToken.value[start_index + 3])
                            {
                            case 'r':
                                switch (currentToken.value[start_index + 4])
                                {
                                case '\0':
                                    currentToken.type = DATA_TYPE; // char
                                    currentToken.line_number = line_number;
                                    break;
                                default:
                                    currentToken.type = IDENTIFIER;
                                    currentToken.line_number = line_number;
                                    break;
                                }
                                break;
                            case 'n':
                                switch (currentToken.value[start_index + 4])
                                {
                                case 'c':
                                    switch (currentToken.value[start_index + 5])
                                    {
                                    case 'e':
                                        switch (currentToken.value[start_index + 6])
                                        {
                                        case '\0':
                                            currentToken.type = KEYWORD; // chance
                                            currentToken.line_number = line_number;
                                            break;
                                        default:
                                            currentToken.type = IDENTIFIER;
                                            currentToken.line_number = line_number;
                                            break;
                                        }
                                        break;
                                    default:
                                        currentToken.type = IDENTIFIER;
                                        currentToken.line_number = line_number;
                                        break;
                                    }
                                    break;
                                default:
                                    currentToken.type = IDENTIFIER;
                                    currentToken.line_number = line_number;
                                    break;
                                }
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                currentToken.line_number = line_number;
                                break;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                            break;
                        }
                        break;
                    case 'o':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 'n':
                            switch (currentToken.value[start_index + 3])
                            {
                            case 's':
                                switch (currentToken.value[start_index + 4])
                                {
                                case 't':
                                    switch (currentToken.value[start_index + 5])
                                    {
                                    case '\0':
                                        currentToken.type = KEYWORD; // const
                                        currentToken.line_number = line_number;
                                        break;
                                    default:
                                        currentToken.type = IDENTIFIER;
                                        currentToken.line_number = line_number;
                                        break;
                                    }
                                    break;
                                default:
                                    currentToken.type = IDENTIFIER;
                                    currentToken.line_number = line_number;
                                    break;
                                }
                                break;
                            case 't':
                                switch (currentToken.value[start_index + 4])
                                {
                                case 'i':
                                    switch (currentToken.value[start_index + 5])
                                    {
                                    case 'n':
                                        switch (currentToken.value[start_index + 6])
                                        {
                                        case 'u':
                                            switch (currentToken.value[start_index + 7])
                                            {
                                            case 'e':
                                                switch (currentToken.value[start_index + 8])
                                                {
                                                case '\0':
                                                    currentToken.type = KEYWORD; // continue
                                                    currentToken.line_number = line_number;
                                                    break;
                                                default:
                                                    currentToken.type = IDENTIFIER;
                                                    currentToken.line_number = line_number;
                                                    break;
                                                }
                                                break;
                                            default:
                                                currentToken.type = IDENTIFIER;
                                                currentToken.line_number = line_number;
                                                break;
                                            }
                                            break;
                                        default:
                                            currentToken.type = IDENTIFIER;
                                            currentToken.line_number = line_number;
                                            break;
                                        }
                                        break;
                                    default:
                                        currentToken.type = IDENTIFIER;
                                        currentToken.line_number = line_number;
                                        break;
                                    }
                                    break;
                                default:
                                    currentToken.type = IDENTIFIER;
                                    currentToken.line_number = line_number;
                                    break;
                                }
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                currentToken.line_number = line_number;
                                break;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                            break;
                        }
                        break;
                    default:
                        currentToken.type = IDENTIFIER;
                        currentToken.line_number = line_number;
                        break;
                    }
                    break;
                case 'd':
                    switch (currentToken.value[start_index + 1])
                    {
                    case 'e':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 'f':
                            switch (currentToken.value[start_index + 3])
                            {
                            case '\0':
                                currentToken.type = KEYWORD; // def
                                currentToken.line_number = line_number;
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                currentToken.line_number = line_number;
                                break;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                            break;
                        }
                        break;
                    default:
                        currentToken.type = IDENTIFIER;
                        currentToken.line_number = line_number;
                        break;
                    }
                    break;
                case 'e':
                    switch (currentToken.value[start_index + 1])
                    {
                    case 'l':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 'i':
                            switch (currentToken.value[start_index + 3])
                            {
                            case 'f':
                                switch (currentToken.value[start_index + 4])
                                {
                                case '\0':
                                    currentToken.type = KEYWORD; // elif
                                    currentToken.line_number = line_number;
                                    break;
                                default:
                                    currentToken.type = IDENTIFIER;
                                    currentToken.line_number = line_number;
                                    break;
                                }
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                currentToken.line_number = line_number;
                                break;
                            }
                            break;
                        case 's':
                            switch (currentToken.value[start_index + 3])
                            {
                            case 'e':
                                switch (currentToken.value[start_index + 4])
                                {
                                case '\0':
                                    currentToken.type = KEYWORD; // else
                                    currentToken.line_number = line_number;
                                    break;
                                default:
                                    currentToken.type = IDENTIFIER;
                                    currentToken.line_number = line_number;
                                    break;
                                }
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                currentToken.line_number = line_number;
                                break;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                            break;
                        }
                        break;
                    case 'x':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 't':
                            switch (currentToken.value[start_index + 3])
                            {
                            case 'e':
                                switch (currentToken.value[start_index + 4])
                                {
                                case 'n':
                                    switch (currentToken.value[start_index + 5])
                                    {
                                    case 's':
                                        switch (currentToken.value[start_index + 6])
                                        {
                                        case 'i':
                                            switch (currentToken.value[start_index + 7])
                                            {
                                            case 'o':
                                                switch (currentToken.value[start_index + 8])
                                                {
                                                case 'n':
                                                    switch (currentToken.value[start_index + 9])
                                                    {
                                                    case '\0':
                                                        currentToken.type = RESERVED_WORDS; // extension
                                                        currentToken.line_number = line_number;
                                                        break;
                                                    default:
                                                        currentToken.type = IDENTIFIER;
                                                        currentToken.line_number = line_number;
                                                        break;
                                                    }
                                                    break;
                                                default:
                                                    currentToken.type = IDENTIFIER;
                                                    currentToken.line_number = line_number;
                                                    break;
                                                }
                                                break;
                                            default:
                                                currentToken.type = IDENTIFIER;
                                                currentToken.line_number = line_number;
                                                break;
                                            }
                                            break;
                                        default:
                                            currentToken.type = IDENTIFIER;
                                            currentToken.line_number = line_number;
                                            break;
                                        }
                                        break;
                                    default:
                                        currentToken.type = IDENTIFIER;
                                        currentToken.line_number = line_number;
                                        break;
                                    }
                                    break;
                                case 'r':
                                    switch (currentToken.value[start_index + 5])
                                    {
                                    case 'n':
                                        switch (currentToken.value[start_index + 6])
                                        {
                                        case '\0':
                                            currentToken.type = NOISE_WORDS; // extern
                                            currentToken.line_number = line_number;
                                            break;
                                        default:
                                            currentToken.type = IDENTIFIER;
                                            currentToken.line_number = line_number;
                                            break;
                                        }
                                        break;
                                    default:
                                        currentToken.type = IDENTIFIER;
                                        currentToken.line_number = line_number;
                                        break;
                                    }
                                    break;
                                default:
                                    currentToken.type = IDENTIFIER;
                                    currentToken.line_number = line_number;
                                    break;
                                }
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                currentToken.line_number = line_number;
                                break;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                            break;
                        }
                        break;
                    default:
                        currentToken.type = IDENTIFIER;
                        currentToken.line_number = line_number;
                        break;
                    }
                    break;
                case 'f':
                    switch (currentToken.value[start_index + 1])
                    {
                    case 'a':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 'l':
                            switch (currentToken.value[start_index + 3])
                            {
                            case 's':
                                switch (currentToken.value[start_index + 4])
                                {
                                case 'e':
                                    switch (currentToken.value[start_index + 5])
                                    {
                                    case '\0':
                                        currentToken.type = RESERVED_WORDS; // false
                                        break;
                                    default:
                                        currentToken.type = IDENTIFIER;
                                        break;
                                    }
                                    break;
                                default:
                                    currentToken.type = IDENTIFIER;
                                    break;
                                }
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                break;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            break;
                        }
                        break;
                    case 'l':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 'o':
                            switch (currentToken.value[start_index + 3])
                            {
                            case 'a':
                                switch (currentToken.value[start_index + 4])
                                {
                                case 't':
                                    switch (currentToken.value[start_index + 5])
                                    {
                                    case '\0':
                                        currentToken.type = DATA_TYPE; // float
                                        break;
                                    default:
                                        currentToken.type = IDENTIFIER;
                                        break;
                                    }
                                    break;
                                default:
                                    currentToken.type = IDENTIFIER;
                                    break;
                                }
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                break;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            break;
                        }
                        break;
                    case 'o':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 'r':
                            switch (currentToken.value[start_index + 3])
                            {
                            case '\0':
                                currentToken.type = KEYWORD; // for
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                break;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            break;
                        }
                        break;
                    default:
                        currentToken.type = IDENTIFIER;
                        break;
                    }
                    break;
                case 'p':
                    switch (currentToken.value[start_index + 1])
                    {
                    case 'r':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 'i':
                            switch (currentToken.value[start_index + 3])
                            {
                            case 'n':
                                switch (currentToken.value[start_index + 4])
                                {
                                case 't':
                                    if (currentToken.value[start_index + 5] == '\0')
                                    {
                                        currentToken.type = KEYWORD; // print
                                        currentToken.line_number = line_number;
                                    }
                                    else
                                    {
                                        currentToken.type = IDENTIFIER;
                                        currentToken.line_number = line_number;
                                    }
                                    break;
                                default:
                                    currentToken.type = IDENTIFIER;
                                    currentToken.line_number = line_number;
                                }
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                currentToken.line_number = line_number;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                        }
                        break;
                    default:
                        currentToken.type = IDENTIFIER;
                        currentToken.line_number = line_number;
                    }
                    break;
                case 'r':
                    switch (currentToken.value[start_index + 1])
                    {
                    case 'e':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 't':
                            switch (currentToken.value[start_index + 3])
                            {
                            case 'u':
                                switch (currentToken.value[start_index + 4])
                                {
                                case 'r':
                                    switch (currentToken.value[start_index + 5])
                                    {
                                    case 'n':
                                        switch (currentToken.value[start_index + 6])
                                        {
                                        case '\0':
                                            currentToken.type = KEYWORD; // return
                                            currentToken.line_number = line_number;
                                            break;
                                        default:
                                            currentToken.type = IDENTIFIER;
                                            currentToken.line_number = line_number;
                                            break;
                                        }
                                        break;
                                    default:
                                        currentToken.type = IDENTIFIER;
                                        currentToken.line_number = line_number;
                                        break;
                                    }
                                    break;
                                default:
                                    currentToken.type = IDENTIFIER;
                                    currentToken.line_number = line_number;
                                    break;
                                }
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                currentToken.line_number = line_number;
                                break;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                            break;
                        }
                        break;
                    default:
                        currentToken.type = IDENTIFIER;
                        currentToken.line_number = line_number;
                        break;
                    }
                    break;
                case 's':
                    switch (currentToken.value[start_index + 1])
                    {
                    case 't':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 'r':
                            switch (currentToken.value[start_index + 3])
                            {
                            case '\0':
                                currentToken.type = DATA_TYPE; // str
                                currentToken.line_number = line_number;
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                currentToken.line_number = line_number;
                                break;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                            break;
                        }
                        break;
                    default:
                        currentToken.type = IDENTIFIER;
                        currentToken.line_number = line_number;
                        break;
                    }
                    break;
                case 't':
                    switch (currentToken.value[start_index + 1])
                    {
                    case 'r':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 'u':
                            switch (currentToken.value[start_index + 3])
                            {
                            case 'e':
                                switch (currentToken.value[start_index + 4])
                                {
                                case '\0':
                                    currentToken.type = RESERVED_WORDS; // true
                                    currentToken.line_number = line_number;
                                    break;
                                default:
                                    currentToken.type = IDENTIFIER;
                                    currentToken.line_number = line_number;
                                    break;
                                }
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                currentToken.line_number = line_number;
                                break;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                            break;
                        }
                        break;
                    default:
                        currentToken.type = IDENTIFIER;
                        currentToken.line_number = line_number;
                        break;
                    }
                    break;
                case 'u':
                    switch (currentToken.value[start_index + 1])
                    {
                    case 's':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 'i':
                            switch (currentToken.value[start_index + 3])
                            {
                            case 'n':
                                switch (currentToken.value[start_index + 4])
                                {
                                case 'g':
                                    switch (currentToken.value[start_index + 5])
                                    {
                                    case '\0':
                                        currentToken.type = RESERVED_WORDS; // using
                                        currentToken.line_number = line_number;
                                        break;
                                    default:
                                        currentToken.type = IDENTIFIER;
                                        currentToken.line_number = line_number;
                                        break;
                                    }
                                    break;
                                default:
                                    currentToken.type = IDENTIFIER;
                                    currentToken.line_number = line_number;
                                    break;
                                }
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                currentToken.line_number = line_number;
                                break;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                            break;
                        }
                        break;
                    default:
                        currentToken.type = IDENTIFIER;
                        currentToken.line_number = line_number;
                        break;
                    }
                    break;
                case 'v':
                    switch (currentToken.value[start_index + 1])
                    {
                    case 'a':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 'l':
                            switch (currentToken.value[start_index + 3])
                            {
                            case 'u':
                                switch (currentToken.value[start_index + 4])
                                {
                                case 'e':
                                    switch (currentToken.value[start_index + 5])
                                    {
                                    case '\0':
                                        currentToken.type = KEYWORD; // value
                                        currentToken.line_number = line_number;
                                        break;
                                    default:
                                        currentToken.type = IDENTIFIER;
                                        currentToken.line_number = line_number;
                                        break;
                                    }
                                    break;
                                default:
                                    currentToken.type = IDENTIFIER;
                                    currentToken.line_number = line_number;
                                    break;
                                }
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                currentToken.line_number = line_number;
                                break;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                            break;
                        }
                        break;
                    default:
                        currentToken.type = IDENTIFIER;
                        currentToken.line_number = line_number;
                        break;
                    }
                    break;
                case 'w':
                    switch (currentToken.value[start_index + 1])
                    {
                    case 'h':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 'i':
                            switch (currentToken.value[start_index + 3])
                            {
                            case 'l':
                                switch (currentToken.value[start_index + 4])
                                {
                                case 'e':
                                    if (currentToken.value[start_index + 5] == '\0')
                                    {
                                        currentToken.type = KEYWORD; // while
                                        currentToken.line_number = line_number;
                                    }
                                    else
                                    {
                                        currentToken.type = IDENTIFIER;
                                        currentToken.line_number = line_number;
                                    }
                                    break;
                                default:
                                    currentToken.type = IDENTIFIER;
                                    currentToken.line_number = line_number;
                                    break;
                                }
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                currentToken.line_number = line_number;
                                break;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                            break;
                        }
                        break;
                    case 'i':
                        switch (currentToken.value[start_index + 2])
                        {
                        case 't':
                            switch (currentToken.value[start_index + 3])
                            {
                            case 'h':
                                if (currentToken.value[start_index + 4] == '\0')
                                {
                                    currentToken.type = KEYWORD; // with
                                    currentToken.line_number = line_number;
                                }
                                else
                                {
                                    currentToken.type = IDENTIFIER;
                                    currentToken.line_number = line_number;
                                }
                                break;
                            default:
                                currentToken.type = IDENTIFIER;
                                currentToken.line_number = line_number;
                                break;
                            }
                            break;
                        default:
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                            break;
                        }
                        break;
                    default:
                        currentToken.type = IDENTIFIER;
                        currentToken.line_number = line_number;
                        break;
                    }
                    break;
                default:
                    currentToken.type = IDENTIFIER;
                    currentToken.line_number = line_number;
                    break;
                }
            }

            printToken(currentToken, file);
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
            currentToken.line_number = line_number;
            printToken(currentToken, file);
        }

        // Handle Arithmetic, Relational, Logical, Assignment, and Unary Operators
        else if (input[i] == '~')
        {
            currentToken.value[0] = input[i];
            currentToken.value[1] = '\0';
            currentToken.type = OPERATOR;
            currentToken.line_number = line_number;

            if (input[i + 1] == '=')
            {
                // Handle the ~= operator (integer division assignment)
                currentToken.value[1] = input[i + 1];
                currentToken.value[2] = '\0';
                i++; // Move past '='
            }

            printToken(currentToken, file);
            i++;
        }

        else if ((input[i] == '=' && input[i + 1] == '='))
        {
            // Handle relational equality (==)
            currentToken.value[0] = input[i];
            currentToken.value[1] = input[i + 1];
            currentToken.value[2] = '\0';
            currentToken.type = OPERATOR;
            currentToken.line_number = line_number;
            printToken(currentToken, file);
            i += 2;
        }
        else if (input[i] == '=')
        {
            // Handle assignment operator (=)
            currentToken.value[0] = input[i];
            currentToken.value[1] = '\0';
            currentToken.type = OPERATOR;
            currentToken.line_number = line_number;
            printToken(currentToken, file);
            i++;
        }

        else if (input[i] == '+' || input[i] == '-')
        {
            currentToken.value[0] = input[i];
            currentToken.value[1] = '\0';
            currentToken.type = OPERATOR;
            currentToken.line_number = line_number;

            if (input[i + 1] == '+')
            {
                currentToken.value[1] = input[i + 1];
                currentToken.value[2] = '\0';
                i++; // Move past '+'
            }
            else if (input[i + 1] == '-')
            {
                currentToken.value[1] = input[i + 1];
                currentToken.value[2] = '\0';
                i++; // Move past '-'
            }
            else if (input[i + 1] == '=')
            {
                currentToken.value[1] = input[i + 1];
                currentToken.value[2] = '\0';
                i++; // Move past '='
            }
            else if (isdigit(input[i + 1]))
            {
                // Handle cases where the operator is followed by a digit (e.g., unary operators like +5 or -3)
            }

            printToken(currentToken, file);
            i++; // Move to the next character
        }
        else if (input[i] == '*' || input[i] == '/' || input[i] == '%' || input[i] == '^')
        {
            currentToken.value[0] = input[i];
            currentToken.value[1] = '\0';
            currentToken.type = OPERATOR;
            currentToken.line_number = line_number;

            if (input[i + 1] == '=')
            {
                currentToken.value[1] = input[i + 1];
                currentToken.value[2] = '\0';
                i++; // Move past '='
            }

            printToken(currentToken, file);
            i++; // Move to the next character
        }

        else if (input[i] == '>')
        {
            // Handle '>' or '>='
            currentToken.value[0] = '>';
            currentToken.value[1] = '\0';
            currentToken.type = OPERATOR;
            currentToken.line_number = line_number;

            // Check if the next character is '=' to form '>='
            if (input[i + 1] == '=')
            {
                currentToken.value[1] = '=';
                currentToken.value[2] = '\0';
                i++; // Move past the '='
            }

            printToken(currentToken, file);
            i++; // Move to the next character
        }
        else if (input[i] == '<')
        {
            // Handle '<' or '<='
            currentToken.value[0] = '<';
            currentToken.value[1] = '\0';
            currentToken.type = OPERATOR;
            currentToken.line_number = line_number;

            // Check if the next character is '=' to form '<='
            if (input[i + 1] == '=')
            {
                currentToken.value[1] = '=';
                currentToken.value[2] = '\0';
                i++; // Move past the '='
            }

            printToken(currentToken, file);
            i++; // Move to the next character
        }
        else if (input[i] == '!')
        {
            // Handle '!='
            currentToken.value[0] = '!';
            currentToken.value[1] = '\0';
            currentToken.type = OPERATOR;
            currentToken.line_number = line_number;

            // Check if the next character is '=' to form '!='
            if (input[i + 1] == '=')
            {
                currentToken.value[1] = '=';
                currentToken.value[2] = '\0';
                i++; // Move past the '='
            }

            printToken(currentToken, file);
            i++; // Move to the next character
        }
        else if (input[i] == '=')
        {
            // Handle '=='
            currentToken.value[0] = '=';
            currentToken.value[1] = '\0';
            currentToken.type = OPERATOR;
            currentToken.line_number = line_number;

            // Check if the next character is '=' to form '=='
            if (input[i + 1] == '=')
            {
                currentToken.value[1] = '=';
                currentToken.value[2] = '\0';
                i++; // Move past the '='
            }

            printToken(currentToken, file);
            i++; // Move to the next character
        }

        else if (input[i] == '&')
        {
            // Handle the case for '&'
            currentToken.value[0] = '&';
            currentToken.value[1] = '\0';
            currentToken.type = OPERATOR;
            currentToken.line_number = line_number;

            // Check if the next character is '&' for '&&'
            if (input[i + 1] == '&')
            {
                currentToken.value[1] = '&';
                currentToken.value[2] = '\0';
                i++; // Move past the second '&'
            }

            printToken(currentToken, file);
            i++; // Move to the next character
        }
        else if (input[i] == '|')
        {
            // Handle the case for '|'
            currentToken.value[0] = '|';
            currentToken.value[1] = '\0';
            currentToken.type = OPERATOR;
            currentToken.line_number = line_number;

            // Check if the next character is '|' for '||'
            if (input[i + 1] == '|')
            {
                currentToken.value[1] = '|';
                currentToken.value[2] = '\0';
                i++; // Move past the second '|'
            }

            printToken(currentToken, file);
            i++; // Move to the next character
        }

        // Handle Logical NOT Operator
        else if (input[i] == '!')
        {
            // Handle logical NOT operator
            currentToken.value[0] = input[i];
            currentToken.value[1] = '\0';
            currentToken.type = OPERATOR;
            currentToken.line_number = line_number;
            printToken(currentToken, file);
            i++;
        }

        // Handle arithmetic operators (+, -, *, /, ~, %, ^)
        else if ((input[i] == '+') ||
                 (input[i] == '-') ||
                 (input[i] == '*') ||
                 (input[i] == '/') ||
                 (input[i] == '~') ||
                 (input[i] == '%') ||
                 (input[i] == '^'))
        {
            // Handle arithmetic operators (+, -, *, /, ~, %, ^)
            currentToken.value[0] = input[i];
            currentToken.value[1] = '\0';
            currentToken.type = OPERATOR;
            currentToken.line_number = line_number;
            printToken(currentToken, file);
            i++;
        }
        // Handle delimiters
        else if (strchr("{};,()[].'\"'", input[i]))
        {
            currentToken.value[0] = input[i];
            currentToken.value[1] = '\0';
            currentToken.type = DELIMITER;
            currentToken.line_number = line_number;
            printToken(currentToken, file);
            i++;
        }

        // Handle unknown characters
        else
        {
            // while (!isspace(input[i]) && input[i] != '\0')
            // {
            //     // Check if the character is an invalid character
            //     if (strchr("@#.`?", input[i])) // Mark as error if character is invalid
            //     {
            //         currentToken.type = ERROR;
            //         currentToken.line_number = line_number;
            //     }
            //     currentToken.value[j++] = input[i++];
            // }
            // currentToken.value[j] = '\0';
            // printToken(currentToken, file);
        }
    }
}

// Check if the file is a .fate file
int isFateFile(const char *filename)
{
    const char *extension = strrchr(filename, '.');
    return (extension != NULL && strcmp(extension, ".fate") == 0);
}

int main()
{
    FILE *file;
    // char *filename = "../FateScript Files/ProgramPresentation.fate";
    // char *filename = "../FateScript Files/sample.fate";
    char filename[1000]; // Buffer to store the filename input
    char fullPath[1024]; // Full path to the file
    char input[2000];
    int i = 0;

    // Prompt the user for the filename (including the extension, e.g., "file.fate")
    printf("Input FateScript file to parse (with extension, e.g., 'file.fate'): ");
    scanf("%999s", filename); // Use %999s to avoid buffer overflow

    // Construct the full file path by concatenating the directory and user input
    snprintf(fullPath, sizeof(fullPath), "../FateScript Files/%s", filename);
    // snprintf(fullPath, sizeof(fullPath), "%s", filename);

    // Check if the file has the .fate extension
    if (!isFateFile(fullPath))
    {
        printf("Error: The file is not a FateScript file.\n");
        return 1;
    }

    // Open the file in write mode for the symbol table
    file = fopen("Symbol Table.txt", "w");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    // Open the source .fate file
    FILE *sourceFile = fopen(fullPath, "r");
    if (sourceFile == NULL)
    {
        perror("Error opening source file");
        fclose(file); // Close symbol table file before exiting
        return 1;
    }

    // Read the content of the file into the input buffer
    i = 0;
    while ((input[i] = fgetc(sourceFile)) != EOF && i < sizeof(input) - 1)
    {
        i++;
    }
    input[i] = '\0';

    // Close the source file after reading
    fclose(sourceFile);

    // Perform lexical analysis
    printf("\nTokens from file '%s':\n", fullPath);
    lexicalAnalyzer(input, file);

    // Close the output file after writing
    fclose(file);

    printf("\nLexical analysis is complete. Output written to 'Symbol Table.txt'.\n");

    return 0;
}