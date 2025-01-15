#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "lexerheader.h"

#define BUFFER_SIZE 1024

// Global file pointer for logging
FILE *log_file;
FILE *symbol_table_file;

// Helper function to print to both console and file
void log_output(const char *format, ...)
{
    va_list args;

    // Print to console
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    // Print to file
    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);
}

// Function declarations for parsing individual components of a declaration
void parse_declaration(char *input, int *index);
void parse_type(char *input, int *index);
void parse_identifier(char *input, int *index);
void parse_assignment(char *input, int *index);
void parse_number(char *input, int *index);
void parse_semicolon(char *input, int *index);

// Parse the full declaration: int <identifier> = <number>;
void parse_declaration(char *input, int *index)
{
    parse_type(input, index);       // Parse type (e.g., "int")
    parse_identifier(input, index); // Parse identifier (e.g., "age")
    parse_assignment(input, index); // Parse assignment operator (e.g., "=")
    parse_number(input, index);     // Parse number (e.g., "12")
    parse_semicolon(input, index);  // Parse semicolon (e.g., ";")
}

// Parse the type part of the declaration (e.g., "int")
void parse_type(char *input, int *index)
{
    if (current_token.type == KEYWORD)
    {
        log_output("Parser: Type token ('%s') processed.\n", current_token.value);
        next_token(input, index);
    }
    else
    {
        log_output("Error: Expected 'int', got '%s'\n", current_token.value);
        exit(1);
    }
}

// Parse the identifier (e.g., "age")
void parse_identifier(char *input, int *index)
{
    if (current_token.type == IDENTIFIER)
    {
        log_output("Parser: Identifier token ('%s') processed.\n", current_token.value);
        next_token(input, index);
    }
    else
    {
        log_output("Error: Expected identifier, got '%s'\n", current_token.value);
        exit(1);
    }
}

// Parse the assignment operator (e.g., "=")
void parse_assignment(char *input, int *index)
{
    if (current_token.type == OPERATOR)
    {
        log_output("Parser: Assignment token ('%s') processed.\n", current_token.value);
        next_token(input, index);
    }
    else
    {
        log_output("Error: Expected '=', got '%s'\n", current_token.value);
        exit(1);
    }
}

// Parse the number (e.g., "12")
void parse_number(char *input, int *index)
{
    if (current_token.type == NUMBER)
    {
        log_output("Parser: Number token ('%s') processed.\n", current_token.value);
        next_token(input, index);
    }
    else
    {
        log_output("Error: Expected number, got '%s'\n", current_token.value);
        exit(1);
    }
}

// Parse the semicolon (e.g., ";")
void parse_semicolon(char *input, int *index)
{
    if (current_token.type == DELIMITER)
    {
        log_output("Parser: Semicolon token ('%s') processed.\n", current_token.value);
        next_token(input, index);
    }
    else
    {
        log_output("Error: Expected ';', got '%s'\n", current_token.value);
        exit(1);
    }
}
int main()
{
    const char *filename = "sample.fate";
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not open file '%s'\n", filename);
        return 1;
    }

    char input[BUFFER_SIZE];
    size_t bytesRead = fread(input, sizeof(char), BUFFER_SIZE - 1, file);
    if (bytesRead == 0 && ferror(file))
    {
        printf("Error: Could not read file '%s'\n", filename);
        fclose(file);
        return 1;
    }

    input[bytesRead] = '\0';
    fclose(file);
    printf("%s\n", input);

    // Open Parse Tree.txt for writing
    log_file = fopen("Parse Tree.txt", "w");
    if (log_file == NULL)
    {
        printf("Error: Could not open file 'Parse Tree.txt' for writing.\n");
        return 1;
    }

    // Open Symbol Table.txt for writing
    symbol_table_file = fopen("Symbol Table.txt", "w");
    if (symbol_table_file == NULL)
    {
        printf("Error: Could not open file 'Symbol Table.txt' for writing.\n");
        fclose(log_file);
        return 1;
    }

    int index = 0;
    printf("Lexer: Starting tokenization process.\n");
    fprintf(symbol_table_file, "Lexer: Starting tokenization process.\n");
    fprintf(symbol_table_file, "%-20s | %-10s\n", "Lexeme", "Token");

    next_token(input, &index);

    printf("Parser: Starting parsing process.\n");
    fprintf(log_file, "Parser: Starting parsing process.\n");

    // Parse declarations in a loop until T_END
    while (current_token.type != END)
    {
        parse_declaration(input, &index);
    }

    // If we reached the end and no errors, log success
    printf("Parser: Declarations parsed successfully from file: %s\n", filename);
    fprintf(log_file, "Parser: Declarations parsed successfully from file: %s\n", filename);

    fclose(log_file);
    fclose(symbol_table_file);
    return 0;
}

// Mac Command
// Move to "Parser and Lexer" Folder Command: cd "Parser and Lexer"
// Compile command: gcc samplelexer.c sampleparser.c -o parser
// Run executable: ./parser
