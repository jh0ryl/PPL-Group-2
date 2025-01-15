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
    printf("Line Number: %d ", line_number);
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
        tokenTypeStr = "RESERVED WORD";
        break;
    case NOISE_WORDS:
        tokenTypeStr = "NOISE WORD";
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
        tokenTypeStr = "STRING LITERAL";
        break;
    case CHARACTER:
        tokenTypeStr = "CHARACTER";
        break;
    case DATA_TYPE:
        tokenTypeStr = "DATA TYPE";
        break;
    default:
        tokenTypeStr = "ERROR";
    }

    fprintf(file, "%-20s %-15s %-10d\n", token.value, tokenTypeStr, token.line_number);
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

        // Handle string literals (single quoted strings)
        else if (currentChar == '\'')
        {
            currentToken.type = CHARACTER;
            currentToken.line_number = line_number;
            j = 0;
            currentToken.value[j++] = currentChar;

            i++;
            while (input[i] != '\'' && input[i] != '\0')
            {
                // Handle escape sequences inside string literals
                if (input[i] == '\\' && (input[i + 1] == '\'' || input[i + 1] == '\\'))
                {
                    currentToken.value[j++] = input[i++];
                    currentToken.value[j++] = input[i++];
                }
                else
                {
                    currentToken.value[j++] = input[i++];
                }
            }

            if (input[i] == '\'')
            {
                currentToken.value[j++] = input[i++];
            }

            currentToken.value[j] = '\0';
            printToken(currentToken, file);
        }

        // Handle identifiers, reserved words, keywords, and noise
        else if (isalpha(currentChar))
        {
            j = 0;
            while (isalnum(input[i]) || input[i] == '_')
            {   
                currentToken.value[j++] = input[i++];
                
            }

           while (!isspace(input[i]) && input[i] != '\0') // Collect until a space or end of input
            {
                if (strchr("@#.`?", input[i])) // If invalid, mark as error
                {
                    error = 1;
                }
                else if (!strchr("@#.`?", input[i]) && ispunct(input[i]))
                {
                    break;
                }
                    currentToken.value[j++] = input[i++];
            }
            currentToken.value[j] = '\0';
            int start_index = 0;
            if (error)
            {
                currentToken.type = ERROR;
                currentToken.line_number = line_number;
            }
            else
            {
                switch (currentToken.value[start_index])
            {
            case 'i': // words that start word with 'i'
                if (currentToken.value[start_index + 1] == 'f' &&
                    currentToken.value[start_index + 2] == '\0')
                {
                    currentToken.type = KEYWORD; // if
                    currentToken.value[j++] = input[i++];
                    currentToken.line_number = line_number;
                }
                else if (currentToken.value[start_index + 1] == 'n')
                {
                    if (currentToken.value[start_index + 2] == 't' &&
                        currentToken.value[start_index + 3] == '\0')
                    {
                        currentToken.type = DATA_TYPE; // int
                        currentToken.line_number = line_number;
                    }
                    else if (currentToken.value[start_index + 2] == 'p' &&
                             currentToken.value[start_index + 3] == 'u' &&
                             currentToken.value[start_index + 4] == 't' &&
                             currentToken.value[start_index + 5] == '\0')
                    {
                        currentToken.type = KEYWORD; // input
                        currentToken.line_number = line_number;
                    }
                    else
                    {
                        currentToken.type = ERROR;
                        currentToken.line_number = line_number;
                    }
                }
                else
                {
                    currentToken.type = ERROR;
                    currentToken.line_number = line_number;
                }
                break;
            case 'a':
                if (currentToken.value[start_index + 1] == 'u' &&
                    currentToken.value[start_index + 2] == 't' &&
                    currentToken.value[start_index + 3] == 'o' &&
                    currentToken.value[start_index + 4] == '\0')
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
            case 'b':
                if (currentToken.value[start_index + 1] == 'o' &&
                    currentToken.value[start_index + 2] == 'o' &&
                    currentToken.value[start_index + 3] == 'l' &&
                    currentToken.value[start_index + 4] == '\0')
                {
                    currentToken.type = DATA_TYPE; // bool
                    currentToken.line_number = line_number;
                }
                else if (currentToken.value[start_index + 1] == 'r' &&
                         currentToken.value[start_index + 2] == 'e' &&
                         currentToken.value[start_index + 3] == 'a' &&
                         currentToken.value[start_index + 4] == 'k' &&
                         currentToken.value[start_index + 5] == '\0')
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
            case 'c':
                if (currentToken.value[start_index + 1] == 'h')
                {
                    if (currentToken.value[start_index + 2] == 'a')
                    {
                        if (currentToken.value[start_index + 3] == 'r' &&
                            currentToken.value[start_index + 4] == '\0')
                        {
                            currentToken.type = DATA_TYPE; // char
                            currentToken.line_number = line_number;
                        }
                        else if (currentToken.value[start_index + 3] == 'n' &&
                                 currentToken.value[start_index + 4] == 'c' &&
                                 currentToken.value[start_index + 5] == 'e' &&
                                 currentToken.value[start_index + 6] == '\0')
                        {
                            currentToken.type = KEYWORD; // chance
                            currentToken.line_number = line_number;
                        }
                        else
                        {
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                        }
                    }
                    else
                    {
                        currentToken.type = IDENTIFIER;
                    }
                }
                else if (currentToken.value[start_index + 1] == 'o')
                {
                    if (currentToken.value[start_index + 2] == 'n')
                    {
                        if (currentToken.value[start_index + 3] == 's' &&
                            currentToken.value[start_index + 4] == 't' &&
                            currentToken.value[start_index + 5] == '\0')
                        {
                            currentToken.type = KEYWORD; // const
                            currentToken.line_number = line_number;
                        }
                        else if (currentToken.value[start_index + 3] == 't' &&
                                 currentToken.value[start_index + 4] == 'i' &&
                                 currentToken.value[start_index + 5] == 'n' &&
                                 currentToken.value[start_index + 6] == 'u' &&
                                 currentToken.value[start_index + 7] == 'e' &&
                                 currentToken.value[start_index + 8] == '\0')
                        {
                            currentToken.type = KEYWORD; // continue
                            currentToken.line_number = line_number;
                        }
                        else
                        {
                            currentToken.type = IDENTIFIER;
                            currentToken.line_number = line_number;
                        }
                    }
                    else
                    {
                        currentToken.type = IDENTIFIER;
                    }
                }
                else
                {
                    currentToken.type = IDENTIFIER;
                    currentToken.line_number = line_number;
                }
                break;
            case 'd':
                if (currentToken.value[start_index + 1] == 'e' &&
                    currentToken.value[start_index + 2] == 'f' &&
                    currentToken.value[start_index + 3] == '\0')
                {
                    currentToken.type = KEYWORD; // def
                    currentToken.line_number = line_number;
                }
                else
                {
                    currentToken.type = IDENTIFIER;
                    currentToken.line_number = line_number;
                }
                break;
            case 'e':
                if (currentToken.value[start_index + 1] == 'l')
                {
                    if (currentToken.value[start_index + 2] == 'i' &&
                        currentToken.value[start_index + 3] == 'f' &&
                        currentToken.value[start_index + 4] == '\0')
                    {
                        currentToken.type = KEYWORD; // elif
                        currentToken.line_number = line_number;
                    }
                    else if (currentToken.value[start_index + 2] == 's' &&
                             currentToken.value[start_index + 3] == 'e' &&
                             currentToken.value[start_index + 4] == '\0')
                    {
                        currentToken.type = KEYWORD; // else
                        currentToken.line_number = line_number;
                    }
                    else
                    {
                        currentToken.type = IDENTIFIER; // else
                        currentToken.line_number = line_number;
                    }
                }
                else if (currentToken.value[start_index + 1] == 'x')
                {
                    if (currentToken.value[start_index + 2] == 't')
                    {
                        if (currentToken.value[start_index + 3] == 'e')
                        {
                            if (currentToken.value[start_index + 4] == 'n' &&
                                currentToken.value[start_index + 5] == 's' &&
                                currentToken.value[start_index + 6] == 'i' &&
                                currentToken.value[start_index + 7] == 'o' &&
                                currentToken.value[start_index + 8] == 'n' &&
                                currentToken.value[start_index + 9] == '\0')
                            {
                                currentToken.type = RESERVED_WORDS; // extension
                                currentToken.line_number = line_number;
                            }
                            else if (currentToken.value[start_index + 4] == 'r' &&
                                     currentToken.value[start_index + 5] == 'n' &&
                                     currentToken.value[start_index + 6] == '\0')
                            {
                                currentToken.type = NOISE_WORDS; // extern
                                currentToken.line_number = line_number;
                            }
                            else
                            {
                                currentToken.type = IDENTIFIER; // extern
                                currentToken.line_number = line_number;
                            }
                        }
                        else
                        {
                            currentToken.type = IDENTIFIER;
                        }
                    }
                    else
                    {
                        currentToken.type = IDENTIFIER;
                    }
                }
                else
                {
                    currentToken.type = IDENTIFIER;
                }
                break;
            case 'f':
                if (currentToken.value[start_index + 1] == 'a' &&
                    currentToken.value[start_index + 2] == 'l' &&
                    currentToken.value[start_index + 3] == 's' &&
                    currentToken.value[start_index + 4] == 'e' &&
                    currentToken.value[start_index + 5] == '\0')
                {
                    currentToken.type = RESERVED_WORDS; // false
                    currentToken.line_number = line_number;
                }
                else if (currentToken.value[start_index + 1] == 'l' &&
                         currentToken.value[start_index + 2] == 'o' &&
                         currentToken.value[start_index + 3] == 'a' &&
                         currentToken.value[start_index + 4] == 't' &&
                         currentToken.value[start_index + 5] == '\0')
                {
                    currentToken.type = DATA_TYPE; // float
                    currentToken.line_number = line_number;
                }
                else if (currentToken.value[start_index + 1] == 'o' &&
                         currentToken.value[start_index + 2] == 'r' &&
                         currentToken.value[start_index + 3] == '\0')
                {
                    currentToken.type = KEYWORD; // for
                    currentToken.line_number = line_number;
                }
                else
                {
                    currentToken.type = IDENTIFIER;
                    currentToken.line_number = line_number;
                }
                break;
            case 'm':
                if (currentToken.value[start_index + 1] == 'o' &&
                    currentToken.value[start_index + 2] == 'd' &&
                    currentToken.value[start_index + 3] == 'u' &&
                    currentToken.value[start_index + 4] == 'l' &&
                    currentToken.value[start_index + 5] == 'e' &&
                    currentToken.value[start_index + 6] == '\0')
                {
                    currentToken.type = RESERVED_WORDS; // module
                    currentToken.line_number = line_number;
                }
                else
                {
                    currentToken.type = IDENTIFIER;
                    currentToken.line_number = line_number;
                }
                break;
            case 'p':
                if (currentToken.value[start_index + 1] == 'r' &&
                    currentToken.value[start_index + 2] == 'i' &&
                    currentToken.value[start_index + 3] == 'n' &&
                    currentToken.value[start_index + 4] == 't' &&
                    currentToken.value[start_index + 5] == '\0')
                {
                    currentToken.type = KEYWORD; // print
                    currentToken.line_number = line_number;
                }
                break;
            case 'r':
                if (currentToken.value[start_index + 1] == 'e' &&
                    currentToken.value[start_index + 2] == 't' &&
                    currentToken.value[start_index + 3] == 'u' &&
                    currentToken.value[start_index + 4] == 'r' &&
                    currentToken.value[start_index + 5] == 'n' &&
                    currentToken.value[start_index + 6] == '\0')
                {
                    currentToken.type = KEYWORD; // return
                    currentToken.line_number = line_number;
                }
                else
                {
                    currentToken.type = IDENTIFIER;
                    currentToken.line_number = line_number;
                }
                break;
            case 's':
                if (currentToken.value[start_index + 1] == 't' &&
                    currentToken.value[start_index + 2] == 'r' &&
                    currentToken.value[start_index + 3] == '\0')
                {
                    currentToken.type = DATA_TYPE; // str
                    currentToken.line_number = line_number;
                }
                else
                {
                    currentToken.type = IDENTIFIER;
                    currentToken.line_number = line_number;
                }
                break;
            case 't':
                if (currentToken.value[start_index + 1] == 'r' &&
                    currentToken.value[start_index + 2] == 'u' &&
                    currentToken.value[start_index + 3] == 'e' &&
                    currentToken.value[start_index + 4] == '\0')
                {
                    currentToken.type = RESERVED_WORDS; // true
                    currentToken.line_number = line_number;
                }
                else
                {
                    currentToken.type = IDENTIFIER;
                    currentToken.line_number = line_number;
                }
                break;
            case 'u':
                if (currentToken.value[start_index + 1] == 's' &&
                    currentToken.value[start_index + 2] == 'i' &&
                    currentToken.value[start_index + 3] == 'n' &&
                    currentToken.value[start_index + 4] == 'g' &&
                    currentToken.value[start_index + 5] == '\0')
                {
                    currentToken.type = RESERVED_WORDS; // using
                    currentToken.line_number = line_number;
                }
                else
                {
                    currentToken.type = IDENTIFIER;
                    currentToken.line_number = line_number;
                }
                break;
            case 'v':
                if (currentToken.value[start_index + 1] == 'a' &&
                    currentToken.value[start_index + 2] == 'l' &&
                    currentToken.value[start_index + 3] == 'u' &&
                    currentToken.value[start_index + 4] == 'e' &&
                    currentToken.value[start_index + 5] == '\0')
                {
                    currentToken.type = KEYWORD; // value
                    currentToken.line_number = line_number;
                }
                else
                {
                    currentToken.type = IDENTIFIER;
                    currentToken.line_number = line_number;
                }
                break;
            case 'w':
                if (currentToken.value[start_index + 1] == 'h' &&
                    currentToken.value[start_index + 2] == 'i' &&
                    currentToken.value[start_index + 3] == 'l' &&
                    currentToken.value[start_index + 4] == 'e' &&
                    currentToken.value[start_index + 5] == '\0')
                {
                    currentToken.type = KEYWORD; // while
                    currentToken.line_number = line_number;
                }
                else if (currentToken.value[start_index + 1] == 'i' &&
                         currentToken.value[start_index + 2] == 't' &&
                         currentToken.value[start_index + 3] == 'h' &&
                         currentToken.value[start_index + 4] == '\0')
                {
                    currentToken.type = KEYWORD; // with
                    currentToken.line_number = line_number;
                }
                else
                {
                    currentToken.type = IDENTIFIER;
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
        else if (input[i] == '~' && input[i + 1] == '=')
        {
            // Handle the ~= operator (integer division assignment)
            currentToken.value[0] = input[i];
            currentToken.value[1] = input[i + 1];
            currentToken.value[2] = '\0';
            currentToken.type = OPERATOR;
            currentToken.line_number = line_number;
            printToken(currentToken, file);
            i += 2;
        }
        else if ((input[i] == '+' && input[i + 1] == '+') ||
                 (input[i] == '-' && input[i + 1] == '-') ||
                 (input[i] == '+' && isdigit(input[i + 1])) ||
                 (input[i] == '-' && isdigit(input[i + 1])))
        {
            // Handle unary operators
            currentToken.value[0] = input[i];
            currentToken.value[1] = input[i + 1];
            currentToken.value[2] = '\0';
            currentToken.type = OPERATOR;
            currentToken.line_number = line_number;
            printToken(currentToken, file);
            i += 2;
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
        else if ((input[i] == '+' && input[i + 1] == '=') ||
                 (input[i] == '-' && input[i + 1] == '=') ||
                 (input[i] == '*' && input[i + 1] == '=') ||
                 (input[i] == '/' && input[i + 1] == '=') ||
                 (input[i] == '%' && input[i + 1] == '=') ||
                 (input[i] == '^' && input[i + 1] == '='))
        {
            // Handle other assignment operators (+=, -=, *=, /=, %=, ^=)
            currentToken.value[0] = input[i];
            currentToken.value[1] = input[i + 1];
            currentToken.value[2] = '\0';
            currentToken.line_number = line_number;
            currentToken.type = OPERATOR;
            printToken(currentToken, file);
            i += 2;
        }
        else if ((input[i] == '>' && input[i + 1] == '=') ||
                 (input[i] == '<' && input[i + 1] == '=') ||
                 (input[i] == '!' && input[i + 1] == '=') ||
                 (input[i] == '=' && input[i + 1] == '='))
        {
            // Handle two-character relational operators (>=, <=, !=, ==)
            currentToken.value[0] = input[i];
            currentToken.value[1] = input[i + 1];
            currentToken.value[2] = '\0';
            currentToken.type = OPERATOR;
            currentToken.line_number = line_number;
            printToken(currentToken, file);
            i += 2;
        }
        else if (input[i] == '>' || input[i] == '<') // Handle single-character relational operators (> or <)
        {
            currentToken.value[0] = input[i];
            currentToken.value[1] = '\0';
            currentToken.type = OPERATOR;
            currentToken.line_number = line_number;
            printToken(currentToken, file);
            i++;
        }
        else if ((input[i] == '&' && input[i + 1] == '&') ||
                 (input[i] == '|' && input[i + 1] == '|'))
        {
            // Handle logical operators (&&, ||)
            currentToken.value[0] = input[i];
            currentToken.value[1] = input[i + 1];
            currentToken.value[2] = '\0';
            currentToken.type = OPERATOR;
            currentToken.line_number = line_number;
            printToken(currentToken, file);
            i += 2;
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
        else if (strchr("{};,()[].'\"'", currentChar))
        {
            currentToken.value[0] = currentChar;
            currentToken.value[1] = '\0';
            currentToken.type = DELIMITER;
            currentToken.line_number = line_number;
            printToken(currentToken, file);
            i++;
        }
        // Handle unknown characters
        else
        {
            currentToken.value[0] = currentChar;
            currentToken.value[1] = '\0';
            currentToken.type = ERROR;
            currentToken.line_number = line_number;
            printToken(currentToken, file);
            i++;
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
    char *filename = "../FateScript Files/sample.fate";
    // char *filename = "FateScript Files/sample.fate";
    // char *filename = "FateScript Files/sample.txt";
    // char *filename = "FateScript Files/delimitersCommentsWhitespace.fate";
    // char *filename = "FateScript Files/keywordsNoiseWordsReservedWords.fate";
    // char *filename = "FateScript Files/operators.fate";
    // char *filename = "FateScript Files/sampleProgram1.fate";
    // char *filename = "FateScript Files/sampleProgram2.fate";
    char input[1000];
    int i = 0;

    // Check if the file has the .fate extension
    if (!isFateFile(filename))
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

    // Write headers to the symbol table
    // fprintf(file, "%-20s %-20s %-20s\n", "Lexeme", "Token", "Line Number");
    // fprintf(file, "-----------------------------------------------------\n");

    // Open the source .fate file
    FILE *sourceFile = fopen(filename, "r");
    if (sourceFile == NULL)
    {
        perror("Error opening source file");
        return 1;
    }

    // Read the content of the file into input buffer
    i = 0;
    while ((input[i] = fgetc(sourceFile)) != EOF && i < sizeof(input) - 1)
    {
        i++;
    }
    input[i] = '\0';

    // Close the source file after reading
    fclose(sourceFile);

    printf("\nTokens from file '%s':\n", filename);
    lexicalAnalyzer(input, file);

    // Close the output file after writing
    fclose(file);

    printf("\nLexical analysis is complete. Output written to 'Symbol Table.txt'.\n");

    return 0;
}
