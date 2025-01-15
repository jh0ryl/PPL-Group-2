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
    int line_number;
} Token;

int line_number = 1;

// Function to classify and print token
void printToken(Token token, FILE *file)
{
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
        tokenTypeStr = "Keyword";
        break;
    case RESERVED_WORDS:
        tokenTypeStr = "Reserved Word";
        break;
    case NOISE_WORDS:
        tokenTypeStr = "Noise Word";
        break;
    case IDENTIFIER:
        tokenTypeStr = "Identifier";
        break;
    case NUMBER:
        tokenTypeStr = "Number";
        break;
    case OPERATOR:
        tokenTypeStr = "Operator";
        break;
    case DELIMITER:
        tokenTypeStr = "Delimiter";
        break;
    case COMMENT:
        tokenTypeStr = "Comment";
        break;
    case WHITESPACE:
        tokenTypeStr = "Whitespace";
        break;
    case STRING_LITERALS:
        tokenTypeStr = "String Literal";
        break;
    case CHARACTER:
        tokenTypeStr = "Character";
        break;
    case DATA_TYPE:
        tokenTypeStr = "Data Type";
        break;
    default:
        tokenTypeStr = "Error";
    }

    fprintf(file, "%-20s | %-15s | %-10d\n", token.value, tokenTypeStr, token.line_number);
}

// Lexical analyzer function
void lexicalAnalyzer(const char *input, FILE *file)
{
    int i = 0, j = 0;
    char currentChar;
    Token currentToken;
    currentToken.line_number = line_number;

    while ((currentChar = input[i]) != '\0')
    {
        //  Actual Whitespace
        if (isspace(currentChar))
        {
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
            printToken(currentToken, file);
            i += 2;
        }

        //  Single-line comments
        if (currentChar == '#')
        {
            currentToken.type = COMMENT;
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
            currentToken.value[j] = '\0';
            int start_index = 0;
            switch (currentToken.value[start_index])
            {
            case 'i': // words that start word with 'i'
                if (currentToken.value[start_index + 1] == 'f' &&
                    currentToken.value[start_index + 2] == '\0')
                {
                    currentToken.type = KEYWORD; // if
                    // currentToken.value[0] = strdup('if');
                }
                else if (currentToken.value[start_index + 1] == 'n')
                {
                    if (currentToken.value[start_index + 2] == 't' &&
                        currentToken.value[start_index + 3] == '\0')
                    {
                        currentToken.type = DATA_TYPE; // int
                    }
                    else if (currentToken.value[start_index + 2] == 'p' &&
                             currentToken.value[start_index + 3] == 'u' &&
                             currentToken.value[start_index + 4] == 't' &&
                             currentToken.value[start_index + 5] == '\0')
                    {
                        currentToken.type = KEYWORD; // input
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
            case 'a':
                if (currentToken.value[start_index + 1] == 'u' &&
                    currentToken.value[start_index + 2] == 't' &&
                    currentToken.value[start_index + 3] == 'o' &&
                    currentToken.value[start_index + 4] == '\0')
                {
                    currentToken.type = NOISE_WORDS; // auto
                }
                else
                {
                    currentToken.type = IDENTIFIER;
                }
                break;
            case 'b':
                if (currentToken.value[start_index + 1] == 'o' &&
                    currentToken.value[start_index + 2] == 'o' &&
                    currentToken.value[start_index + 3] == 'l' &&
                    currentToken.value[start_index + 4] == '\0')
                {
                    currentToken.type = DATA_TYPE; // bool
                }
                else if (currentToken.value[start_index + 1] == 'r' &&
                         currentToken.value[start_index + 2] == 'e' &&
                         currentToken.value[start_index + 3] == 'a' &&
                         currentToken.value[start_index + 4] == 'k' &&
                         currentToken.value[start_index + 5] == '\0')
                {
                    currentToken.type = KEYWORD; // break
                }
                else
                {
                    currentToken.type = IDENTIFIER;
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
                        }
                        else if (currentToken.value[start_index + 3] == 'n' &&
                                 currentToken.value[start_index + 4] == 'c' &&
                                 currentToken.value[start_index + 5] == 'e' &&
                                 currentToken.value[start_index + 6] == '\0')
                        {
                            currentToken.type = KEYWORD; // chance
                        }
                        else
                        {
                            currentToken.type = IDENTIFIER;
                        }
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
                        }
                        else if (currentToken.value[start_index + 3] == 't' &&
                                 currentToken.value[start_index + 4] == 'i' &&
                                 currentToken.value[start_index + 5] == 'n' &&
                                 currentToken.value[start_index + 6] == 'u' &&
                                 currentToken.value[start_index + 7] == 'e' &&
                                 currentToken.value[start_index + 8] == '\0')
                        {
                            currentToken.type = KEYWORD; // continue
                        }
                        else
                        {
                            currentToken.type = IDENTIFIER;
                        }
                    }
                }
                else
                {
                    currentToken.type = IDENTIFIER;
                }
                break;
            case 'd':
                if (currentToken.value[start_index + 1] == 'e' &&
                    currentToken.value[start_index + 2] == 'f' &&
                    currentToken.value[start_index + 3] == '\0')
                {
                    currentToken.type = KEYWORD; // def
                }
                else
                {
                    currentToken.type = IDENTIFIER;
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
                    }
                    else if (currentToken.value[start_index + 2] == 's' &&
                             currentToken.value[start_index + 3] == 'e' &&
                             currentToken.value[start_index + 4] == '\0')
                    {
                        currentToken.type = KEYWORD; // else
                    }
                    else
                    {
                        currentToken.type = IDENTIFIER; // else
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
                            }
                            else if (currentToken.value[start_index + 4] == 'r' &&
                                     currentToken.value[start_index + 5] == 'n' &&
                                     currentToken.value[start_index + 6] == '\0')
                            {
                                currentToken.type = NOISE_WORDS; // extern
                            }
                            else
                            {
                                currentToken.type = IDENTIFIER; // extern
                            }
                        }
                    }
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
                }
                else if (currentToken.value[start_index + 1] == 'l' &&
                         currentToken.value[start_index + 2] == 'o' &&
                         currentToken.value[start_index + 3] == 'a' &&
                         currentToken.value[start_index + 4] == 't' &&
                         currentToken.value[start_index + 5] == '\0')
                {
                    currentToken.type = DATA_TYPE; // float
                }
                else if (currentToken.value[start_index + 1] == 'o' &&
                         currentToken.value[start_index + 2] == 'r' &&
                         currentToken.value[start_index + 3] == '\0')
                {
                    currentToken.type = KEYWORD; // for
                }
                else
                {
                    currentToken.type = IDENTIFIER;
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
                }
                else
                {
                    currentToken.type = IDENTIFIER;
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
                }
                else
                {
                    currentToken.type = IDENTIFIER;
                }
                break;
            case 's':
                if (currentToken.value[start_index + 1] == 't' &&
                    currentToken.value[start_index + 2] == 'r' &&
                    currentToken.value[start_index + 3] == '\0')
                {
                    currentToken.type = DATA_TYPE; // str
                }
                else
                {
                    currentToken.type = IDENTIFIER;
                }
                break;
            case 't':
                if (currentToken.value[start_index + 1] == 'r' &&
                    currentToken.value[start_index + 2] == 'u' &&
                    currentToken.value[start_index + 3] == 'e' &&
                    currentToken.value[start_index + 4] == '\0')
                {
                    currentToken.type = RESERVED_WORDS; // true
                }
                else
                {
                    currentToken.type = IDENTIFIER;
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
                }
                else
                {
                    currentToken.type = IDENTIFIER;
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
                }
                else
                {
                    currentToken.type = IDENTIFIER;
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
                }
                else if (currentToken.value[start_index + 1] == 'i' &&
                         currentToken.value[start_index + 2] == 't' &&
                         currentToken.value[start_index + 3] == 'h' &&
                         currentToken.value[start_index + 4] == '\0')
                {
                    currentToken.type = KEYWORD; // with
                }

                break;
            default:
                currentToken.type = IDENTIFIER;
                break;
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
            printToken(currentToken, file);
            i += 2;
        }
        else if (input[i] == '=')
        {
            // Handle assignment operator (=)
            currentToken.value[0] = input[i];
            currentToken.value[1] = '\0';
            currentToken.type = OPERATOR;
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
            printToken(currentToken, file);
            i += 2;
        }
        else if (input[i] == '>' || input[i] == '<') // Handle single-character relational operators (> or <)
        {
            currentToken.value[0] = input[i];
            currentToken.value[1] = '\0';
            currentToken.type = OPERATOR;
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
            printToken(currentToken, file);
            i++;
        }
        // Handle next line
        else if (strchr(";{}", currentChar))
        {
            currentToken.value[0] = currentChar;
            currentToken.value[1] = '\0';
            currentToken.type = DELIMITER;
            printToken(currentToken, file);
            line_number++;
            i++;
        }
        // Handle delimiters
        else if (strchr(";,()[].'\"'", currentChar))
        {
            currentToken.value[0] = currentChar;
            currentToken.value[1] = '\0';
            currentToken.type = DELIMITER;
            printToken(currentToken, file);
            i++;
        }
        // Handle unknown characters
        else
        {
            currentToken.value[0] = currentChar;
            currentToken.value[1] = '\0';
            currentToken.type = ERROR;
            printToken(currentToken, file);
            i++;
        }
        printf("Line Number: %d\n", line_number);
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
    fprintf(file, "%-20s %-20s %-20s\n", "Lexeme", "Token", "Line Number");
    fprintf(file, "-----------------------------------------------------\n");

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
