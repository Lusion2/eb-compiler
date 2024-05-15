#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "token.h"
#include "../typedefs.h"
#include "../dynlist.h"

void tokenize(dynlist_token *tokens, dynlist_char *code){
    // Initialize a buffer to parse for keywords
    dynlist_char buffer;

    // Initializing the char
    char c;
    for(int i = 0; i < (int)code->size; i++){
        dynlist_free(buffer);
        dynlist_init(buffer, char);
        // Set c equal to the current character
        c = code->data[i];
        
        if(isspace(c)){
            continue;
        }
        // If the character is a letter, then begin parsing for a keyword
        else if(isalpha(c)){
            // Put current char into buffer
            dynlist_push(buffer, c);

            // Continue checking if the char is alpha-numeric and saving it to the buffer
            if(!(i++ < (int)code->size)){
                fprintf(stderr, "Expected token\n");
                dynlist_free(buffer);
                dynlist_free_ptr(code);
                dynlist_free_ptr(tokens);
                exit(EXIT_FAILURE);
            }
            c = code->data[i];
            while(isalnum(c)){
                dynlist_push(buffer, c);
                i++;
                c = code->data[i];
            }
            dynlist_push(buffer, '\0');
            i--;

            // Check if the buffer is a keyword, if it is add the token to the pointer array of tokens
            if(strcmp(buffer.data, "exit") == 0){
                Token token = { .type = TokenType_exit_, .val = "" };
                dynlist_push_ptr(tokens, token);
            }
            else if(strcmp(buffer.data, "int") == 0){
                Token token = {.type = TokenType_int, .val = ""};
                dynlist_push_ptr(tokens, token);
            }
            else{
                // It's an identifier
                Token token = {.type = TokenType_ident };
                token.val = (char *)malloc(buffer.size);
                strncpy(token.val, buffer.data, buffer.size);
                dynlist_push_ptr(tokens, token);

            }
        }
        else if(isdigit(c)){
            // Put current char into buffer
            dynlist_push(buffer, c);

            if(!(i++ < (int)code->size)){
                fprintf(stderr, "Expected token\n");
                dynlist_free(buffer);
                dynlist_free_ptr(code);
                dynlist_free_ptr(tokens);
                exit(EXIT_FAILURE);
            }
            // Continue checking if c is a digit
            c = code->data[i];
            while(isdigit(c)){
                dynlist_push(buffer, c);
                i++;
                c = code->data[i];
            }
            dynlist_push(buffer, '\0');
            i--;

            // store the digit as a value in a token
            Token token = {.type = TokenType_int_lit};
            token.val = (char *)malloc(buffer.size);
            strncpy(token.val, buffer.data, buffer.size);
            dynlist_push_ptr(tokens, token);
        }
        else if(c == '('){
            // Add an open paren token
            Token token = {.type = TokenType_open_paren};
            dynlist_push_ptr(tokens, token);
        }
        else if(c == ')'){
            // Add an close paren token
            Token token = {.type = TokenType_close_paren};
            dynlist_push_ptr(tokens, token);
        }
        else if(c == ';'){
            // Add a semicolon token
            Token token = {.type = TokenType_semi};
            dynlist_push_ptr(tokens, token);
        }
        else if(c == '+'){
            Token token = {.type = TokenType_plus};
            dynlist_push_ptr(tokens, token);
        }
        else if(c == '='){
            Token token = {.type = TokenType_equ};
            dynlist_push_ptr(tokens, token);
        }
        else{
            fprintf(stderr, "Character '%c' is an invalid token\n", c);
            exit(EXIT_FAILURE);
        }
    }
    for(size_t i = 0; i < tokens->size; i++){
        if(tokens->data[i].type == TokenType_ident){
            // printf("%s\n", tokens->data[i].val);
        }
        if(tokens->data[i].type == TokenType_int_lit){
            // printf("%s\n", tokens->data[i].val);
        }
    }

    dynlist_free(buffer);
}