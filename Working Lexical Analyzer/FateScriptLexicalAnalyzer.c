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
    ERROR
} TokenType;

// Keywords list
const char *keywords[] = {"int", "float", "bool", "str", "char", "if", "else", "return", "while", "elif", "for", "continue", "break", "const", "def", "print", "input", "with", "chance", "value"};
int isKeyword(const char *str)
{
    for (int i = 0; i < 20; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// Reserved words
const char *reservedWords[] = {"using", "extension", "module", "true", "false"};
int isReservedWord(const char *str)
{
    for (int i = 0; i < 5; i++)
    {
        if (strcmp(str, reservedWords[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// Noise words
const char *noiseWords[] = {"extern", "auto"};
int isNoiseWord(const char *str)
{
    for (int i = 0; i < 2; i++)
    {
        if (strcmp(str, noiseWords[i]) == 0)
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
    default:
        printf("Error: Unknown token %s\n", token.value);
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
    default:
        tokenTypeStr = "Error";
    }

    fprintf(file, "%-20s | %s\n", token.value, tokenTypeStr);
}

// Lexical analyzer function
void lexicalAnalyzer(const char *input, FILE *file)
{
    int i = 0, j = 0;
    char currentChar;
    Token currentToken;

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

        // Handle identifiers, reserved words, keywords, and noise
        else if (isalpha(currentChar))
        {
            j = 0;
            while (isalnum(input[i]) || input[i] == '_')
            {
                currentToken.value[j++] = input[i++];
            }
            currentToken.value[j] = '\0';

            if (isReservedWord(currentToken.value))
            {
                currentToken.type = RESERVED_WORDS;
            }
            else if (isKeyword(currentToken.value))
            {
                currentToken.type = KEYWORD;
            }
            else if (isNoiseWord(currentToken.value))
            {
                currentToken.type = NOISE_WORDS;
            }
            else
            {
                currentToken.type = IDENTIFIER;
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
        // Handle delimiters
        else if (strchr(";,(){}[].'\"'", currentChar))
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
    char *filename = "../FateScript Files/complete.fate";
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
    fprintf(file, "%-20s %-20s\n", "Lexeme", "Token");
    fprintf(file, "----------------------------------------\n");

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
