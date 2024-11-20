#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_SIZE 100

// Define possible token types
typedef enum
{
    KEYWORD,
    RESERVED_WORDS,
    IDENTIFIER,
    NUMBER,
    OPERATOR,
    DELIMITER,
    COMMENT,
    ERROR
} TokenType;

// Keywords list
const char *keywords[] = {"int", "float", "bool", "str", "char", "if", "else", "return", "while", "elif", "for", "continue", "break", "const", "def", "true", "false", "print", "input"};
int isKeyword(const char *str)
{
    for (int i = 0; i < 19; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// Reserved words list
const char *reservedWords[] = {"async"};
int isReservedWord(const char *str)
{
    for (int i = 0; i < 1; i++)
    {
        if (strcmp(str, reservedWords[i]) == 0)
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
    default:
        printf("Error: Unknown token %s\n", token.value);
    }

    // Write to the file
    switch (token.type)
    {
    case KEYWORD:
        fprintf(file, "Keyword: %s\n", token.value);
        break;
    case RESERVED_WORDS:
        fprintf(file, "Reserved Word: %s\n", token.value);
        break;
    case IDENTIFIER:
        fprintf(file, "Identifier: %s\n", token.value);
        break;
    case NUMBER:
        fprintf(file, "Number: %s\n", token.value);
        break;
    case OPERATOR:
        fprintf(file, "Operator: %s\n", token.value);
        break;
    case DELIMITER:
        fprintf(file, "Delimiter: %s\n", token.value);
        break;
    case COMMENT:
        fprintf(file, "Comment: %s\n", token.value);
        break;
    default:
        fprintf(file, "Error: Unknown token %s\n", token.value);
    }
}

// Lexical analyzer function
void lexicalAnalyzer(const char *input, FILE *file)
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
            printToken(currentToken, file);
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
            printToken(currentToken, file);
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
            printToken(currentToken, file);
        }
        // Handle identifiers, reserved words, and keywords
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
        // Handle operators
        else if (strchr("+-*/=<>", currentChar))
        {
            currentToken.value[0] = currentChar;
            currentToken.value[1] = '\0';
            currentToken.type = OPERATOR;
            printToken(currentToken, file);
            i++;
        }
        // Handle delimiters
        else if (strchr(";,(){}", currentChar))
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
    // Check the file extension
    const char *extension = strrchr(filename, '.');
    return (extension != NULL && strcmp(extension, ".fate") == 0);
}

int main()
{
    FILE *file;
    char *filename = "FateScript Files/sample.fate"; // Path to the file
    char input[1000];
    int i = 0;

    // Check if the file has the .fate extension
    if (!isFateFile(filename))
    {
        printf("Error: The file is not a FateScript file.\n");
        return 1;
    }

    // Open the file in write mode for the symbol table
    file = fopen("Symbol Table.txt", "w"); // Open for writing
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

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
    input[i] = '\0'; // Null-terminate the string

    // Close the source file after reading
    fclose(sourceFile);

    printf("\nTokens from file '%s':\n", filename);
    lexicalAnalyzer(input, file); // Perform lexical analysis and write to the file

    // Close the output file after writing
    fclose(file);

    printf("\nLexical analysis is complete. Output written to 'Symbol Table.txt'.\n");

    return 0;
}
