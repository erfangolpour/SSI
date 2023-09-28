#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tokenize the input string with strtok and support for quotes (double and single) (includes the quotes in the tokens)
char** tokenize(char* input) {
    const char* space_delimiters = " \t\n";  // Token delimiter for spaces: space, tab, newline
    const char* quote_delimiters = "\"'";    // Token delimiter for quotes

    int token_count = 0;
    int max_token_count = 20;
    char** tokens = (char**)malloc(max_token_count * sizeof(char*));
    if (tokens == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    // Tokenize without strtok
    char* current_char = input;
    char* current_token = NULL;
    while (*current_char != '\0') {
        if (strchr(space_delimiters, *current_char) != NULL) {
            // End of token
            if (current_token != NULL) {
                // Terminate the token
                *current_char = '\0';
                // Add the token to the array
                tokens[token_count++] = current_token;
                current_token = NULL;
            }
        } else if (strchr(quote_delimiters, *current_char) != NULL) {
            // Find the corresponding quote
            char* quote = strchr(current_char + 1, *current_char);

            int token_was_empty = current_token == NULL;
            int space_after_quote = quote != NULL && strchr(space_delimiters, *(quote + 1)) != NULL;
            if (token_was_empty) {
                // Skip the quote only if it's part of a string token
                current_token = space_after_quote ? current_char + 1 : current_char;
            }

            if (quote != NULL) {
                // Corresponding quote found, add string token to the array
                if (space_after_quote && token_was_empty) {
                    *quote = ' '; // this will turn into a null terminator in the next iteration
                    current_char = quote - 1;
                } else {
                    current_char = quote;
                }
            }
        } else {
            // Add the character to the current token
            if (current_token == NULL) {
                current_token = current_char;
            }
        }

        if (token_count - 1 > max_token_count) {
            // This also takes into account the last token and the NULL terminator
            // Double the size of the array if necessary
            max_token_count *= 2;
            tokens = (char**)realloc(tokens, max_token_count * sizeof(char*));
            if (tokens == NULL) {
                fprintf(stderr, "Memory allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        ++current_char;
    }
    if (current_token != NULL) {
        // Add the last token
        tokens[token_count++] = current_token;
    }

    tokens[token_count] = NULL;  // Null-terminate the token array
    return tokens;
}