#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_SIZE 1000
#define BUFFER_SIZE 1024

int statement_number = 1;

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

typedef struct
{
    TokenType type;
    char value[MAX_TOKEN_SIZE];
    int line_number;
} Token;

int get_token(FILE *input_file, Token *current_token);

// Function to map string to TokenType enum
TokenType get_token_type(const char *type_str)
{
    if (strcmp(type_str, "KEYWORD") == 0)
        return KEYWORD;
    if (strcmp(type_str, "RESERVED_WORDS") == 0)
        return RESERVED_WORDS;
    if (strcmp(type_str, "NOISE_WORDS") == 0)
        return NOISE_WORDS;
    if (strcmp(type_str, "IDENTIFIER") == 0)
        return IDENTIFIER;
    if (strcmp(type_str, "NUMBER") == 0)
        return NUMBER;
    if (strcmp(type_str, "OPERATOR") == 0)
        return OPERATOR;
    if (strcmp(type_str, "DELIMITER") == 0)
        return DELIMITER;
    if (strcmp(type_str, "COMMENT") == 0)
        return COMMENT;
    if (strcmp(type_str, "WHITESPACE") == 0)
        return WHITESPACE;
    if (strcmp(type_str, "STRING_LITERALS") == 0)
        return STRING_LITERALS;
    if (strcmp(type_str, "CHARACTER") == 0)
        return CHARACTER;
    if (strcmp(type_str, "DATA_TYPE") == 0)
        return DATA_TYPE;
    return ERROR;
}

// Parsing Functions Prototypes
void parse_assignment(FILE *input_file, FILE *output_file, Token *current_token);
void determine_statement(FILE *input_file, FILE *output_file, Token *current_token);

// Function to map TokenType enum to string
const char *get_token_type_string(TokenType type)
{
    switch (type)
    {
    case KEYWORD:
        return "KEYWORD";
    case RESERVED_WORDS:
        return "RESERVED_WORDS";
    case NOISE_WORDS:
        return "NOISE_WORDS";
    case IDENTIFIER:
        return "IDENTIFIER";
    case NUMBER:
        return "NUMBER";
    case OPERATOR:
        return "OPERATOR";
    case DELIMITER:
        return "DELIMITER";
    case COMMENT:
        return "COMMENT";
    case WHITESPACE:
        return "WHITESPACE";
    case STRING_LITERALS:
        return "STRING_LITERALS";
    case CHARACTER:
        return "CHARACTER";
    case DATA_TYPE:
        return "DATA TYPE";
    default:
        return "ERROR";
    }
}

// Get the next token from the input file
int get_token(FILE *input_file, Token *current_token)
{
    char line[BUFFER_SIZE];
    if (fgets(line, sizeof(line), input_file) == NULL)
        return 0;

    char value[MAX_TOKEN_SIZE], type_str[MAX_TOKEN_SIZE];
    int line_number;

    // Check for string literals (handling "Hello, World!")
    if (sscanf(line, "\"%[^\"]\" %s %d", value, type_str, &line_number) == 3)
    {
        // It's a string literal, including the quotes in the value
        current_token->type = STRING_LITERALS;

        // Add the double quotes to the string value
        snprintf(current_token->value, MAX_TOKEN_SIZE, "\"%s\"", value);

        current_token->line_number = line_number;
    }

    else if (sscanf(line, "%s %s %d", value, type_str, &line_number) == 3)
    {
        // It's a normal token (non-string)
        current_token->type = get_token_type(type_str);
        strncpy(current_token->value, value, MAX_TOKEN_SIZE);
        current_token->line_number = line_number;
    }
    else
    {
        return 0; // Invalid token
    }
    return 1;
}

// Function to handle data type
void parse_data_type(FILE *input_file, FILE *output_file, Token *current_token)
{
    // Check if the current token is a data type
    if (current_token->type == DATA_TYPE)
    {
        fprintf(output_file, "\tDATA TYPE: ('%s')\n", current_token->value);
        get_token(input_file, current_token); // Get next token
    }
    else
    {
        fprintf(output_file, "Error (Line %d): Expected DATA TYPE for declarative statement\n", current_token->line_number);
    }
}

// Function to handle identifier
void parse_identifier(FILE *input_file, FILE *output_file, Token *current_token)
{
    // Check if the current token is an identifier
    if (current_token->type == IDENTIFIER)
    {
        fprintf(output_file, "\tIDENTIFIER: ('%s')\n", current_token->value);
        get_token(input_file, current_token); // Get next token
    }
    else
    {
        fprintf(output_file, "Error (Line %d): Expected IDENTIFIER\n", current_token->line_number);
    }
}

// Function to handle semicolon
void parse_semicolon(FILE *input_file, FILE *output_file, Token *current_token)
{
    // Check for the SEMICOLON token
    if (current_token->type == DELIMITER && strcmp(current_token->value, ";") == 0)
    {
        fprintf(output_file, "\tSEMICOLON: ('%s')\n", current_token->value);
    }
    else
    {
        fprintf(output_file, "Error (Line %d): Expected SEMICOLON (';') after identifier\n", current_token->line_number);
    }
}

// Function to handle the assignment operator
void parse_assignment_operator(FILE *input_file, FILE *output_file, Token *current_token)
{

    if (current_token->type == OPERATOR && strcmp(current_token->value, "=") == 0)
    {
        fprintf(output_file, "\tASSIGNMENT OPERATOR: ('%s')\n", current_token->value);
        get_token(input_file, current_token); // Get next token
    }
    else
    {
        fprintf(output_file, "Error (Line %d): Expected '=' after identifier in assignment statement\n", current_token->line_number);
        return;
    }
}

// Function to parse the operator in a condition
void parse_operator(FILE *input_file, FILE *output_file, Token *current_token)
{
    // Check if the current token is a valid operator
    if (current_token->type == OPERATOR)
    {
        fprintf(output_file, "\tOPERATOR: ('%s')\n", current_token->value);
        get_token(input_file, current_token);
    }
    else
    {
        fprintf(output_file, "Error (Line %d): Expected valid operator in condition\n", current_token->line_number);
        return;
    }
}

// Helper function to parse string literals
void parse_string_literal(FILE *input_file, FILE *output_file, Token *current_token)
{
    // Check for STRING_LITERALS token type and print it
    if (current_token->type == STRING_LITERALS)
    {
        fprintf(output_file, "\tSTRING: (%s)\n", current_token->value);
        get_token(input_file, current_token); // Get next token
    }
    else
    {
        fprintf(output_file, "Error (Line %d): Expected string literal\n", current_token->line_number);
    }
}

// Helper function to parse string literals
void parse_character(FILE *input_file, FILE *output_file, Token *current_token)
{
    // Check for STRING_LITERALS token type and print it
    if (current_token->type == CHARACTER)
    {
        fprintf(output_file, "\tCHARACTER: (%s)\n", current_token->value);
        get_token(input_file, current_token); // Get next token
    }
    else
    {
        fprintf(output_file, "Error (Line %d): Expected Character\n", current_token->line_number);
    }
}

void parse_number(FILE *input_file, FILE *output_file, Token *current_token)
{
    // Print the number value
    fprintf(output_file, "\tNUMBER: ('%s')\n", current_token->value);

    // Get the next token after number to proceed
    get_token(input_file, current_token);
}

void parse_expression(FILE *input_file, FILE *output_file, Token *current_token)
{
    // Check for opening parenthesis
    if (current_token->type == DELIMITER && strcmp(current_token->value, "(") == 0)
    {
        fprintf(output_file, "\tPARENTHESIS: ('%s')\n", current_token->value);
        get_token(input_file, current_token); // Get the next token

        parse_expression(input_file, output_file, current_token); // Parse the nested expression

        // Ensure a closing parenthesis exists
        if (current_token->type == DELIMITER && strcmp(current_token->value, ")") == 0)
        {
            fprintf(output_file, "\tPARENTHESIS: ('%s')\n", current_token->value);
            get_token(input_file, current_token); // Move to the next token
        }
        else
        {
            fprintf(output_file, "Error (Line %d): Missing closing parenthesis\n", current_token->line_number);
            return;
        }
    }
    else if (current_token->type == IDENTIFIER)
    {
        parse_identifier(input_file, output_file, current_token);
    }
    else if (current_token->type == NUMBER)
    {
        parse_number(input_file, output_file, current_token);
    }
    else if (current_token->type == STRING_LITERALS)
    {
        parse_string_literal(input_file, output_file, current_token);
    }
    else if (current_token->type == CHARACTER)
    {
        parse_character(input_file, output_file, current_token);
    }
    else
    {
        fprintf(output_file, "Error (Line %d): Expected a value, identifier, or nested expression\n", current_token->line_number);
        return;
    }

    // Check for operators and recursively parse the right-hand side
    while (current_token->type == OPERATOR)
    {
        fprintf(output_file, "\tOPERATOR: ('%s')\n", current_token->value);
        get_token(input_file, current_token); // Move to the next token

        // Recursively parse the next part of the expression
        parse_expression(input_file, output_file, current_token);
    }
}

void parse_condition(FILE *input_file, FILE *output_file, Token *current_token)
{
    fprintf(output_file, "Statement %d (Line %d): Condition\n", statement_number++, current_token->line_number);

    // Parse the identifier (e.g., i)
    parse_identifier(input_file, output_file, current_token);

    // Parse the operator (e.g., <=, =)
    parse_operator(input_file, output_file, current_token);

    // Parse the number or second identifier (e.g., 5 or another variable)
    if (current_token->type == NUMBER)
    {
        parse_number(input_file, output_file, current_token);
    }
    else if (current_token->type == IDENTIFIER)
    {
        parse_identifier(input_file, output_file, current_token);
    }
    else
    {
        fprintf(output_file, "Error (Line %d): Expected number or identifier after operator in condition\n", current_token->line_number);
        return;
    }
}

// Function to parse the increment in the loop
void parse_increment(FILE *input_file, FILE *output_file, Token *current_token)
{
    fprintf(output_file, "Statement %d (Line %d): Increment\n", statement_number++, current_token->line_number);

    // Parse identifier (variable to increment)
    parse_identifier(input_file, output_file, current_token);

    parse_operator(input_file, output_file, current_token);
}

// Statement Parsing Functions
void parse_declaration(FILE *input_file, FILE *output_file, Token *current_token)
{
    // Print the declarative statement with statement number and line number
    fprintf(output_file, "Statement %d (Line %d): Declaration\n", statement_number++, current_token->line_number);

    // Call parse functions
    parse_data_type(input_file, output_file, current_token);

    // Parse the identifier
    parse_identifier(input_file, output_file, current_token);

    // Check for a comma (to handle multiple declarations)
    while (current_token->type == DELIMITER && strcmp(current_token->value, ",") == 0)
    {
        fprintf(output_file, "\tCOMMA: ('%s')\n", current_token->value);
        get_token(input_file, current_token); // Get next token

        parse_identifier(input_file, output_file, current_token);
    }

    // Parse the semicolon
    parse_semicolon(input_file, output_file, current_token);
    fprintf(output_file, "\n");
}

void parse_assignment(FILE *input_file, FILE *output_file, Token *current_token)
{
    // Print the assignment statement with statement number and line number
    fprintf(output_file, "Statement %d (Line %d): Assignment\n", statement_number++, current_token->line_number);
    parse_identifier(input_file, output_file, current_token);

    // Parse assignment operator
    parse_assignment_operator(input_file, output_file, current_token);

    // Parse the expression
    parse_expression(input_file, output_file, current_token);

    // Check for the SEMICOLON token
    parse_semicolon(input_file, output_file, current_token);
    fprintf(output_file, "\n");
}

void parse_output(FILE *input_file, FILE *output_file, Token *current_token)
{
    fprintf(output_file, "Statement %d (Line %d): Output\n", statement_number++, current_token->line_number);
    fprintf(output_file, "\tKEYWORD: ('%s')\n", current_token->value);
    get_token(input_file, current_token); // Get next token

    // Check for opening parenthesis '('
    if (current_token->type == DELIMITER && strcmp(current_token->value, "(") == 0)
    {
        fprintf(output_file, "\tPARENTHESIS: ('%s')\n", current_token->value);
        get_token(input_file, current_token); // Get next token to check for string literal

        // Parse the string literal
        parse_string_literal(input_file, output_file, current_token);

        // Parse optional arguments separated by commas
        while (current_token->type == DELIMITER && strcmp(current_token->value, ",") == 0)
        {
            fprintf(output_file, "\tCOMMA: ('%s')\n", current_token->value);
            get_token(input_file, current_token); // Get next token

            // Check if the token is a valid identifier or number
            if (current_token->type == IDENTIFIER)
            {
                parse_identifier(input_file, output_file, current_token);
            }
            else if (current_token->type == NUMBER)
            {
                parse_number(input_file, output_file, current_token);
            }
            else
            {
                fprintf(output_file, "Error (Line %d): Expected identifier or number after ','\n", current_token->line_number);
                return;
            }
        }

        // Check for closing parenthesis ')'
        if (current_token->type == DELIMITER && strcmp(current_token->value, ")") == 0)
        {
            fprintf(output_file, "\tPARENTHESIS: ('%s')\n", current_token->value);
            get_token(input_file, current_token); // Get next token (to check for semicolon)

            // Parse the semicolon directly
            parse_semicolon(input_file, output_file, current_token);
        }
        else
        {
            fprintf(output_file, "Error (Line %d): Expected closing parenthesis ')'\n", current_token->line_number);
            return;
        }
    }
    else
    {
        fprintf(output_file, "Error (Line %d): Expected opening parenthesis '('\n", current_token->line_number);
        return;
    }

    fprintf(output_file, "\n");
}

void parse_input(FILE *input_file, FILE *output_file, Token *current_token)
{
    fprintf(output_file, "Statement %d (Line %d): Input\n", statement_number++, current_token->line_number);
    fprintf(output_file, "\tKEYWORD: ('%s')\n", current_token->value);
    get_token(input_file, current_token); // Get next token

    // Check for opening parenthesis '('
    if (current_token->type == DELIMITER && strcmp(current_token->value, "(") == 0)
    {
        fprintf(output_file, "\tPARENTHESIS: ('%s')\n", current_token->value);
        get_token(input_file, current_token); // Get next token (to check for string literal)

        // Parse string literal (prompt message inside quotes)
        parse_string_literal(input_file, output_file, current_token);

        if (current_token->type == DELIMITER && strcmp(current_token->value, ",") == 0)
        {
            fprintf(output_file, "\tDELIMITER: ('%s')\n", current_token->value);
            get_token(input_file, current_token); // Get next token (to check for address operator or identifier)

            // Check if it's the address operator '&'
            if (current_token->type == OPERATOR && strcmp(current_token->value, "&") == 0)
            {
                fprintf(output_file, "\tDELIMITER: ('%s')\n", current_token->value);
                get_token(input_file, current_token); // Get next token (to check for identifier)
            }
            else
            {
                fprintf(output_file, "Error (Line %d): Expected address operator ('&') for input variable\n", current_token->line_number);
                return; // Exit as we need the address operator
            }

            // Now parse the identifier (variable to store the input)
            if (current_token->type == IDENTIFIER)
            {
                fprintf(output_file, "\tIDENTIFIER: ('%s')\n", current_token->value);
                get_token(input_file, current_token); // Get next token (to check for closing parenthesis)
            }
            else
            {
                fprintf(output_file, "Error (Line %d): Expected identifier for result variable\n", current_token->line_number);
                return; // Exit as identifier is missing
            }

            // Check for closing parenthesis ')'
            if (current_token->type == DELIMITER && strcmp(current_token->value, ")") == 0)
            {
                fprintf(output_file, "\tPARENTHESIS: ('%s')\n", current_token->value);
                get_token(input_file, current_token);                    // Get next token (to check for semicolon)
                parse_semicolon(input_file, output_file, current_token); // Assume semicolon function is already defined
            }
            else
            {
                fprintf(output_file, "Error (Line %d): Expected closing parenthesis ')'\n", current_token->line_number);
            }
        }
        else
        {
            fprintf(output_file, "Error (Line %d): Expected comma (',') after input prompt\n", current_token->line_number);
        }
    }
    else
    {
        fprintf(output_file, "Error (Line %d): Expected opening parenthesis '('\n", current_token->line_number);
    }
    fprintf(output_file, "\n");
}

void parse_for_loop(FILE *input_file, FILE *output_file, Token *current_token)
{
    fprintf(output_file, "Statement %d (Line %d): Iterative (for)\n", statement_number++, current_token->line_number);
    fprintf(output_file, "\tKEYWORD: ('%s')\n", current_token->value);
    get_token(input_file, current_token); // Get next token

    // Check for opening parenthesis '('
    fprintf(output_file, "\t");
    if (current_token->type == DELIMITER && strcmp(current_token->value, "(") == 0)
    {
        fprintf(output_file, "\tPARENTHESIS: ('%s')\n", current_token->value);
        fprintf(output_file, "\n");
        get_token(input_file, current_token); // Get next token

        parse_assignment(input_file, output_file, current_token);
        get_token(input_file, current_token); // Get next token
        parse_condition(input_file, output_file, current_token);
        parse_semicolon(input_file, output_file, current_token);
        fprintf(output_file, "\n");
        get_token(input_file, current_token); // Get next token
        parse_increment(input_file, output_file, current_token);

        // Check for closing parenthesis ')'
        if (current_token->type == DELIMITER && strcmp(current_token->value, ")") == 0)
        {
            fprintf(output_file, "\tPARENTHESIS: ('%s')\n", current_token->value);
            get_token(input_file, current_token); // Get next token
        }
        else
        {
            fprintf(output_file, "Error (Line %d): Missing closing parenthesis in 'for' loop\n", current_token->line_number);
            return;
        }

        // Check for opening curly brace '{'
        if (current_token->type == DELIMITER && strcmp(current_token->value, "{") == 0)
        {
            fprintf(output_file, "\tCURLY BRACE: ('%s')\n", current_token->value);
            fprintf(output_file, "\n");
            get_token(input_file, current_token); // Get next token

            // Parse statements inside the loop
            while (current_token->type != DELIMITER || strcmp(current_token->value, "}") != 0)
            {
                determine_statement(input_file, output_file, current_token);
                get_token(input_file, current_token); // Get next token
            }

            // Check for closing curly brace '}'
            if (current_token->type == DELIMITER && strcmp(current_token->value, "}") == 0)
            {
                fprintf(output_file, "\tCURLY BRACE: ('%s')\n", current_token->value);
            }
            else
            {
                fprintf(output_file, "Error (Line %d): Missing closing curly brace in 'for' loop\n", current_token->line_number);
            }
        }
        else
        {
            fprintf(output_file, "Error (Line %d): Expected opening curly brace '{' in 'for' loop\n", current_token->line_number);
        }
    }
    else
    {
        fprintf(output_file, "Error (Line %d): Expected opening parenthesis '(' after 'for'\n", current_token->line_number);
    }

    fprintf(output_file, "\n");
}

char *peek_next_token(FILE *input_file, Token *current_token)
{
    static Token temp_token;                         // Temporary token for peeking
    long current_pos = ftell(input_file);            // Save the current file position
    int result = get_token(input_file, &temp_token); // Try to fetch the next token

    if (result == 0)
    { // End-of-file or error
        return NULL;
    }

    fseek(input_file, current_pos, SEEK_SET); // Restore file position
    return temp_token.value;                  // Return the peeked token's value
}

void parse_while_loop(FILE *input_file, FILE *output_file, Token *current_token)
{
    fprintf(output_file, "Statement %d (Line %d): Iterative (while)\n", statement_number++, current_token->line_number);
    fprintf(output_file, "\tKEYWORD: ('%s')\n", current_token->value);
    get_token(input_file, current_token); // Get next token

    // open parenthesis
    fprintf(output_file, "\t");
    if (current_token->type == DELIMITER && strcmp(current_token->value, "(") == 0)
    {
        fprintf(output_file, "\tPARENTHESIS: ('%s')\n", current_token->value);
        fprintf(output_file, "\n");
        get_token(input_file, current_token); // Get next token

        parse_condition(input_file, output_file, current_token);
        fprintf(output_file, "\n");

        // Check for closing parenthesis ')'
        if (current_token->type == DELIMITER && strcmp(current_token->value, ")") == 0)
        {
            fprintf(output_file, "\tPARENTHESIS: ('%s')\n", current_token->value);
            get_token(input_file, current_token); // Get next token
        }
        else
        {
            fprintf(output_file, "Error (Line %d): Missing closing parenthesis in 'while' loop\n", current_token->line_number);
            return;
        }

        // Check for opening curly brace '{'
        if (current_token->type == DELIMITER && strcmp(current_token->value, "{") == 0)
        {
            fprintf(output_file, "\tCURLY BRACE: ('%s')\n", current_token->value);
            fprintf(output_file, "\n");
            get_token(input_file, current_token); // Get next token

            while (current_token->type != DELIMITER || strcmp(current_token->value, "}") != 0)
            {
                // Check if the token is a variable and followed by '++' or '--'
                if (current_token->type == IDENTIFIER)
                {
                    char *next_value = peek_next_token(input_file, current_token);
                    if (strcmp(next_value, "++") == 0 || strcmp(next_value, "--") == 0)
                    {
                        // fprintf(output_file, "%s \n", current_token->value);
                        parse_increment(input_file, output_file, current_token); // Handle increment/decrement
                        parse_semicolon(input_file, output_file, current_token); // Handle semicolon
                    }
                    else
                    {
                        determine_statement(input_file, output_file, current_token); // Handle assignments or other statements
                    }
                }
                else
                {
                    determine_statement(input_file, output_file, current_token); // Handle other statements
                }
                get_token(input_file, current_token); // Get next token
            }

            // Check for closing curly brace '}'
            if (current_token->type == DELIMITER && strcmp(current_token->value, "}") == 0)
            {
                fprintf(output_file, "\n");
                fprintf(output_file, "\tCURLY BRACE: ('%s')\n", current_token->value);
            }
            else
            {
                fprintf(output_file, "Error (Line %d): Missing closing curly.\n", current_token->line_number);
            }
        }
        else
        {
            fprintf(output_file, "Error (Line %d): Expected opening curly brace '{' in 'for' loop\n", current_token->line_number);
        }
    }
    else
    {
        fprintf(output_file, "Error (Line %d): Expected opening parenthesis '(' after 'while'\n", current_token->line_number);
    }
    fprintf(output_file, "\n");
}

void parse_if_condition(FILE *input_file, FILE *output_file, Token *current_token)
{
    fprintf(output_file, "Statement %d (Line %d): Conditional (if-statement)\n", statement_number++, current_token->line_number);
    fprintf(output_file, "\tKEYWORD: ('%s')\n", current_token->value);
    get_token(input_file, current_token); // Get next token

    // Check for opening parenthesis '('
    if (current_token->type == DELIMITER && strcmp(current_token->value, "(") == 0)
    {
        fprintf(output_file, "\tPARENTHESIS: ('%s')\n", current_token->value);
        fprintf(output_file, "\n");
        get_token(input_file, current_token); // Get next token

        parse_condition(input_file, output_file, current_token);

        // Check for closing parenthesis ')'
        if (current_token->type == DELIMITER && strcmp(current_token->value, ")") == 0)
        {
            fprintf(output_file, "\tPARENTHESIS: ('%s')\n", current_token->value);
            get_token(input_file, current_token); // Get next token
        }
        else
        {
            fprintf(output_file, "Error (Line %d): Missing closing parenthesis in 'if' statement\n", current_token->line_number);
            return;
        }

        // Check for opening curly brace '{'
        if (current_token->type == DELIMITER && strcmp(current_token->value, "{") == 0)
        {
            fprintf(output_file, "\tCURLY BRACE: ('%s')\n", current_token->value);
            fprintf(output_file, "\n");
            get_token(input_file, current_token); // Get next token

            // Parse statements inside the block
            while (current_token->type != DELIMITER || strcmp(current_token->value, "}") != 0)
            {
                determine_statement(input_file, output_file, current_token);
                get_token(input_file, current_token); // Get next token
            }

            // Check for closing curly brace '}'
            if (current_token->type == DELIMITER && strcmp(current_token->value, "}") == 0)
            {
                fprintf(output_file, "\tCURLY BRACE: ('%s')\n", current_token->value);
                fprintf(output_file, "\n");
            }
            else
            {
                fprintf(output_file, "Error (Line %d): Missing closing curly brace in 'if' statement\n", current_token->line_number);
                return;
            }

            // get_token(input_file, current_token); // Move to next token after block
            // Check for "else"
            if (current_token->type == KEYWORD && strcmp(current_token->value, "else") == 0)
            {
                fprintf(output_file, "Statement %d (Line %d): Conditional (else-statement)\n", statement_number++, current_token->line_number);
                fprintf(output_file, "\tKEYWORD: ('%s')\n", current_token->value);
                fprintf(output_file, "\n");
                get_token(input_file, current_token); // Get next token

                // Check if "else" is followed by "if"
                if (current_token->type == KEYWORD && strcmp(current_token->value, "if") == 0)
                {
                    parse_if_condition(input_file, output_file, current_token); // Recursive call for "else if"
                }
                else if (current_token->type == DELIMITER && strcmp(current_token->value, "{") == 0)
                {
                    fprintf(output_file, "\tCURLY BRACE: ('%s')\n", current_token->value);
                    fprintf(output_file, "\n");
                    get_token(input_file, current_token); // Get next token

                    // Parse statements inside the "else" block
                    while (current_token->type != DELIMITER || strcmp(current_token->value, "}") != 0)
                    {
                        determine_statement(input_file, output_file, current_token);
                        get_token(input_file, current_token); // Get next token
                    }

                    // Check for closing curly brace '}'
                    if (current_token->type == DELIMITER && strcmp(current_token->value, "}") == 0)
                    {
                        fprintf(output_file, "\tCURLY BRACE: ('%s')\n", current_token->value);
                    }
                    else
                    {
                        fprintf(output_file, "Error (Line %d): Missing closing curly brace in 'else' block\n", current_token->line_number);
                        return;
                    }
                }
                else
                {
                    fprintf(output_file, "Error (Line %d): Expected '{' after 'else'\n", current_token->line_number);
                    return;
                }
            }
        }
        else
        {
            fprintf(output_file, "Error (Line %d): Expected opening curly brace '{' in 'if' statement\n", current_token->line_number);
        }
    }
    else
    {
        fprintf(output_file, "Error (Line %d): Expected opening parenthesis '(' after 'if'\n", current_token->line_number);
    }

    fprintf(output_file, "\n");
}

// Function to determine the type of statement and call appropriate parsing functions
void determine_statement(FILE *input_file, FILE *output_file, Token *current_token)
{
    printf("Token ('%s') Line: %d\n", current_token->value, current_token->line_number);

    // Check if it is a declarative statement
    if (current_token->type == DATA_TYPE)
    {
        parse_declaration(input_file, output_file, current_token);
    }
    // Check if it is an assignment statement
    else if (current_token->type == IDENTIFIER)
    {
        parse_assignment(input_file, output_file, current_token);
    }
    // Check if it is an output statement
    else if (current_token->type == KEYWORD && strcmp(current_token->value, "print") == 0)
    {
        parse_output(input_file, output_file, current_token);
    }
    // Check if it is an input statement
    else if (current_token->type == KEYWORD && strcmp(current_token->value, "input") == 0)
    {
        parse_input(input_file, output_file, current_token);
    }
    // Check if it is an output statement
    else if (current_token->type == KEYWORD && strcmp(current_token->value, "for") == 0)
    {
        parse_for_loop(input_file, output_file, current_token);
    }
    // Check if it is an input statement
    else if (current_token->type == KEYWORD && strcmp(current_token->value, "while") == 0)
    {
        parse_while_loop(input_file, output_file, current_token);
    }
    // Check if it is an output statement
    else if (current_token->type == KEYWORD && strcmp(current_token->value, "if") == 0)
    {
        parse_if_condition(input_file, output_file, current_token);
    }
}

int main()
{
    const char *input_filename = "Symbol Table.txt";
    const char *output_filename = "Parse Tree.txt";

    FILE *input_file = fopen(input_filename, "r");
    if (input_file == NULL)
    {
        printf("Error: Could not open file '%s'\n", input_filename);
        return 1;
    }

    FILE *output_file = fopen(output_filename, "w");
    if (output_file == NULL)
    {
        printf("Error: Could not open file '%s' for writing\n", output_filename);
        fclose(input_file);
        return 1;
    }

    Token current_token;

    while (get_token(input_file, &current_token))
    {
        // Use the determine_statement function to process the token
        determine_statement(input_file, output_file, &current_token);
    }

    fclose(input_file);
    fclose(output_file);

    printf("Parsing completed, results written to '%s'\n", output_filename);
    return 0;
}
