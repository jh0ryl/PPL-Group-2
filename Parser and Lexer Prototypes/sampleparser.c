#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "lexerheader.h"

#define BUFFER_SIZE 1024

// Global file pointer for logging
FILE *log_file;
FILE *symbol_table_file;

int line_number = 1;      // Start from line 1
int statement_number = 1; // Start from statement 1

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

// Statement checker
void parse_statement(char *input, int *index);

// Parse statement types
void parse_declaration(char *input, int *index);
void parse_assignment(char *input, int *index);
void parse_print_statement(char *input, int *index);
// void input_statement(char *input, int *index);
// void if_statement(char *input, int *index);
// void else_statement(char *input, int *index);
// void else_if_statement(char *input, int *index);
// void for_loop_statement(char *input, int *index);
// void while_loop_statement(char *input, int *index);

// Parse Components
void parse_term(char *input, int *index);
void parse_expression(char *input, int *index);
void parse_data_type(char *input, int *index);
void parse_identifier(char *input, int *index);
void parse_number(char *input, int *index);
void parse_semicolon(char *input, int *index);
void parse_string_literal(char *input, int *index);

void parse_declaration(char *input, int *index)
{
    log_output("\nStatement: Declaration\n");

    parse_data_type(input, index); // Parse type (e.g., "int")

    // Loop to parse multiple identifiers separated by commas
    while (current_token.type == IDENTIFIER)
    {
        parse_identifier(input, index); // Parse identifier (e.g., "age")

        // Check if there is a comma separating identifiers
        if (current_token.type == OPERATOR && strcmp(current_token.value, ",") == 0)
        {
            log_output("\tOPERATOR: %s\n", current_token.value); // Log the comma
            next_token(input, index);                            // Move to the next token (next identifier)
        }
        else
        {
            break; // Exit the loop when no more identifiers or commas are found
        }
    }

    parse_semicolon(input, index); // Parse semicolon (e.g., ";")
}

// Function to parse the full assignment statement: a = b + c;
void parse_assignment(char *input, int *index)
{
    log_output("\nStatement: Assignment\n");

    // Parse the left-hand side (identifier)
    parse_identifier(input, index); // This should parse 'a'

    // Ensure the current token is the assignment operator "="
    if (current_token.type == OPERATOR && strcmp(current_token.value, "=") == 0)
    {
        log_output("\tOPERATOR: %s\n", current_token.value); // Log "=" operator
        next_token(input, index);                            // Move to the next token
    }
    else
    {
        log_output("Error: Expected '=', got '%s'.\n", current_token.value);
        exit(1); // Exit on error
    }

    // Parse the right-hand side expression (it could be a complex expression like 'cost + tax + 5')
    parse_expression(input, index); // Now we call a function to parse the right-hand side expression

    // Parse the semicolon at the end of the statement
    parse_semicolon(input, index); // Parse ';'
}

// Function to parse an expression (e.g., 'cost + tax + 5')
void parse_expression(char *input, int *index)
{
    log_output("\tExpression: ");

    // First parse the first term (which could be an identifier or a number)
    log_output("\n\t");
    parse_term(input, index);

    // After the first term, we might have operators like '+' or '-'
    while (current_token.type == OPERATOR &&
           (strcmp(current_token.value, "+") == 0 || strcmp(current_token.value, "-") == 0))
    {
        log_output("\t");
        log_output("\tOPERATOR: %s\n", current_token.value); // Log operator

        next_token(input, index); // Move to the next token
        log_output("\t");
        parse_term(input, index); // Parse the next term
    }
}

// Function to parse a term, which could be an identifier or number
void parse_term(char *input, int *index)
{
    if (current_token.type == IDENTIFIER)
    {
        log_output("\tIDENTIFIER: %s\n", current_token.value);
        next_token(input, index); // Move to the next token
    }
    else if (current_token.type == NUMBER)
    {
        log_output("\tNUMBER: %s\n", current_token.value);
        next_token(input, index); // Move to the next token
    }
    else
    {
        log_output("Error: Expected identifier or number, got '%s'.\n", current_token.value);
        exit(1); // Exit on error
    }
}

// Parse a print statement with multiple arguments
void parse_print_statement(char *input, int *index)
{
    log_output("\nStatement: Output\n");

    // Ensure current token is 'print'
    if (current_token.type == KEYWORD && strcmp(current_token.value, "print") == 0)
    {
        log_output("\tOUTPUT: ('%s')\n", current_token.value);
        next_token(input, index); // Move to next token

        // Ensure the current token is an opening parenthesis '('
        if (current_token.type == DELIMITER && strcmp(current_token.value, "(") == 0)
        {
            log_output("\tOPEN_PARENTHESIS ('%s')\n", current_token.value);
            next_token(input, index); // Move to next token

            // Parse the first argument (string literal)
            log_output("\t");
                parse_string_literal(input, index);

            // Handle multiple arguments, separated by commas
            while (current_token.type == OPERATOR && strcmp(current_token.value, ",") == 0)
            {
                log_output("\tCOMMA (',')\n", current_token.value);
                next_token(input, index); // Move to next token

                // Parse the next argument (either identifier or expression)
                if (current_token.type == IDENTIFIER)
                {
                    log_output("\tIDENTIFIER ('%s')\n", current_token.value);
                    next_token(input, index); // Move to next token
                }
                else if (current_token.type == NUMBER)
                {
                    log_output("\tNUMBER ('%s')\n", current_token.value);
                    next_token(input, index); // Move to next token
                }
                else
                {
                    log_output("Error: Unexpected token '%s'. Expected an identifier or literal.\n", current_token.value);
                    exit(1);
                }
            }

            // Ensure closing parenthesis ')'
            if (current_token.type == DELIMITER && strcmp(current_token.value, ")") == 0)
            {
                log_output("\tCLOSE_PARENTHESIS ('%s')\n", current_token.value);
                next_token(input, index); // Move to next token
            }
            else
            {
                log_output("Error: Expected ')', got '%s'.\n", current_token.value);
                exit(1);
            }
        }
        else
        {
            log_output("Error: Expected '(', got '%s'.\n", current_token.value);
            exit(1);
        }

        // Parse semicolon at the end of the print statement
        parse_semicolon(input, index); // Parse semicolon (e.g., ";")
    }
    else
    {
        log_output("Error: Expected 'print', got '%s'.\n", current_token.value);
        exit(1);
    }
}

// Component Parsing Functions

// Parse data type
void parse_data_type(char *input, int *index)
{
    if (current_token.type == KEYWORD)
    {
        log_output("\tDATA TYPE: ('%s')\n", current_token.value);
        next_token(input, index);
    }
    else
    {
        log_output("Error: Expected a type (e.g., 'int'), got '%s'.\n", current_token.value);
        exit(1);
    }
}

// Parse identifier
void parse_identifier(char *input, int *index)
{
    if (current_token.type == IDENTIFIER)
    {
        log_output("\tIDENTIFIER: ('%s')\n", current_token.value);
        next_token(input, index);
    }
    else
    {
        log_output("Error: Expected identifier, got '%s'.\n", current_token.value);
        exit(1);
    }
}

// Parse the number
void parse_number(char *input, int *index)
{
    if (current_token.type == NUMBER)
    {
        log_output("\tNUMBER('%s')\n", current_token.value);
        next_token(input, index);
    }
    else
    {
        log_output("Error: Expected a number, got '%s'.\n", current_token.value);
        exit(1);
    }
}

// Parse the semicolon (e.g., ";")
void parse_semicolon(char *input, int *index)
{
    if (current_token.type == DELIMITER && strcmp(current_token.value, ";") == 0)
    {
        log_output("\tSEMICOLON ('%s')\n", current_token.value);
        next_token(input, index);

        // Increment the line number when a semicolon is found
        line_number++;
    }
    else
    {
        log_output("Error: Expected ';', got '%s'.\n", current_token.value);
        exit(1);
    }
}

// Parse a string literal (e.g., "Hello World!")
void parse_string_literal(char *input, int *index)
{
    if (current_token.type == STRING_LITERAL)
    {
        log_output("STRING_LITERAL ('%s')\n", current_token.value);
        next_token(input, index);
    }
    else
    {
        log_output("Error: Expected string literal, got '%s'.\n", current_token.value);
        exit(1);
    }
}
// Function to parse any statement
void parse_statement(char *input, int *index)
{
    // Print the current statement with its line number
    log_output("\nStatement %d (Line %d): ", statement_number, line_number);

    // Check the current token to determine the type of statement
    if (current_token.type == IDENTIFIER)
    {
        // Parse an assignment statement
        parse_assignment(input, index);
    }
    else if (current_token.type == KEYWORD)
    {
        if (strcmp(current_token.value, "int") == 0)
        {
            // Parse a declaration statement
            parse_declaration(input, index);
        }
        else if (strcmp(current_token.value, "print") == 0)
        {
            // Parse a print statement
            parse_print_statement(input, index);
        }
        else
        {
            // Handle unrecognized keywords
            log_output("Error: Unrecognized keyword '%s'.\n", current_token.value);
            exit(1);
        }
    }
    else
    {
        // Handle unexpected tokens
        log_output("Error: Unexpected token '%s'. Expected an identifier or a keyword.\n", current_token.value);
        exit(1);
    }

    // Increment statement number after successfully parsing a statement
    statement_number++;
}

int main()
{
    const char *filename = "../FateScript Files/sample.fate";
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

    // Parse statements in a loop until END token
    while (current_token.type != END)
    {
        parse_statement(input, &index); // Dispatch based on the statement type
    }

    // If we reached the end and no errors, log success
    printf("\nParser: Parsing successful: %s\n", filename);
    fprintf(log_file, "\nParser: Parsing successful: %s\n", filename);

    fclose(log_file);
    fclose(symbol_table_file);
    return 0;
}

// Mac Command
// Move to "Parser and Lexer" Folder Command: cd "Parser and Lexer"
// Compile command: gcc samplelexer.c sampleparser.c -o parser
// Run executable: ./parser
