#ifndef EB_TOKEN_H
#define EB_TOKEN_H

#include "../typedefs.h"
#include "../dynlist.h"



typedef enum TokenType{
    TokenType_NULL,         // 0
    TokenType_exit_,        // 1
    TokenType_semi,         // 2
    TokenType_int,          // 3
    TokenType_int_lit,      // 4
    TokenType_open_paren,   // 5
    TokenType_close_paren,  // 6
    TokenType_ident,        // 7 
    TokenType_expr,         // 8
    TokenType_plus,         // 9
    TokenType_sub,          // 10
    TokenType_mult,         // 11
    TokenType_div,          // 12
    TokenType_equ,          // 13
} TokenType;

typedef struct Token{
    TokenType type;
    // char *var_ident;
    char *val;
} Token;

typedef dynlist(Token) dynlist_token;


/*
 * Takes code from load_code() and tokenizes it
 * You need to initialize a token * and then pass it into the function
 * It will return the tokens as a pointer array
 */
void tokenize(dynlist_token *tokens, dynlist_char *code);


#endif