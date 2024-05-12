#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "parse.h"
#include "../token/token.h"
#include "../typedefs.h"
#include "../alloc.h"

int bin_prec(TokenType type){
    switch(type){
        case TokenType_plus:
        case TokenType_sub:
            return 1;
        case TokenType_mult:
        case TokenType_div:
            return 2;
        default:
            return 0;
    }
}

node_term *parse_term(dynlist_token *tokens, u32 *index){
    u32 curr_index = *index;

    if(tokens->data[curr_index].type == TokenType_int_lit){
        Token int_lit = tokens->data[curr_index];
        curr_index++;
        node_term *term_int_lit = NULL;
        HEAP_ALLOC(heap, node_term, term_int_lit);
        term_int_lit->type = TermType_int_lit;
        term_int_lit->int_lit = int_lit;
        *index = curr_index;
        return term_int_lit;
    }
    else if(tokens->data[curr_index].type == TokenType_ident){
        Token ident = tokens->data[curr_index];
        curr_index++;
        node_term *term_ident = NULL;
        HEAP_ALLOC(heap, node_term, term_ident);
        term_ident->type = TermType_ident;
        term_ident->ident = ident;
        *index = curr_index;
        return term_ident;
    }
    else if(tokens->data[curr_index].type == TokenType_open_paren){
        curr_index++;
        *index = curr_index;
        node_expr *expr = NULL;
        HEAP_ALLOC(heap, node_expr, expr);
        expr = parse_expr(tokens, index, 0);
        curr_index = *index;
        if(expr == NULL){
            fprintf(stderr, "Expected expression\n");
            exit(EXIT_FAILURE);
        }
        if(tokens->data[curr_index].type != TokenType_close_paren){
            fprintf(stderr, "Expected ')'");
            exit(EXIT_FAILURE);
        }
        node_term *term_paren = NULL;
        HEAP_ALLOC(heap, node_term, term_paren);
        term_paren->expr = expr;
        term_paren->type = TermType_paren;
        return term_paren;
    }
    else{
        *index = curr_index;
        return NULL;
    }
}

/*
 * By default, min_prec should be set to 0
 */
node_expr *parse_expr(dynlist_token *tokens, u32 *index, int min_prec){
    node_term *term_left = parse_term(tokens, index);
    if(term_left == NULL){
        return NULL;
    }

    node_expr *expr_left = NULL;
    HEAP_ALLOC(heap, node_expr, expr_left);
    expr_left->term = term_left;
    expr_left->type = ExprType_Term;

    u32 curr_index = *index;

    while(true){
        Token curr_tok = tokens->data[curr_index];
        int prec = 0;
        if(curr_tok.type != TokenType_NULL){
            prec = bin_prec(curr_tok.type);
            if(prec == 0 || prec < min_prec){
                break;
            }
        }
        else{
            break;
        }
        Token op = tokens->data[curr_index];
        curr_index++;
        *index = curr_index;
        int next_min_prec = prec + 1;
        node_expr *expr_right = parse_expr(tokens, index, next_min_prec);
        if(expr_right == NULL){
            fprintf(stderr, "Unable to parse expression\n");
            exit(EXIT_FAILURE);
        }

        node_expr *expr = NULL;
        HEAP_ALLOC(heap, node_expr, expr);
        if(op.type == TokenType_plus){
            expr->type = ExprType_Add;
            expr->binary.left = expr_left;
            expr->binary.right = expr_right;
        }
        else if(op.type == TokenType_sub){
            expr->type = ExprType_Sub;
            expr->binary.left = expr_left;
            expr->binary.right = expr_right;
        }
        else if(op.type == TokenType_mult){
            expr->type = ExprType_Mult;
            expr->binary.left = expr_left;
            expr->binary.right = expr_right;
        }
        else if(op.type == TokenType_div){
            expr->type = ExprType_Mult;
            expr->binary.left = expr_left;
            expr->binary.right = expr_right;
        }
        else {
            assert(false); // Unreachable (it should be)
        }
        return expr;
    }

    return expr_left;
}

node_stmt *parse_stmt(dynlist_token *tokens, u32 *index){
    u32 curr_index = *index;

    if(tokens->data[curr_index].type == TokenType_exit_ && 
        tokens->data[curr_index + 1].type == TokenType_open_paren){
            curr_index += 2;
            node_stmt *stmt_exit = NULL;
            HEAP_ALLOC(heap, node_stmt, stmt_exit);
            stmt_exit->type = StmtType_exit;
            if(stmt_exit == NULL){
                fprintf(stderr, "Unable to allocate memory\n");
                exit(EXIT_FAILURE);
            }
            *index = curr_index;
            node_expr *expr = parse_expr(tokens, index, 0);
            if(expr == NULL){
                fprintf(stderr, "Invalid expression\n");
                exit(EXIT_FAILURE);
            }
            stmt_exit->expr = expr;
            curr_index = *index;
            assert(*index + 1 < tokens->size);
            if (tokens->data[curr_index].type != TokenType_close_paren){
                fprintf(stderr, "Expected ')'\n");
                exit(EXIT_FAILURE);
            }
            else if(tokens->data[curr_index + 1].type != TokenType_semi){
                fprintf(stderr, "Expected ';'\n");
                exit(EXIT_FAILURE);
            }
            curr_index++;
            *index = curr_index;
            return stmt_exit;
    }
    else if(tokens->data[curr_index].type == TokenType_int &&
    tokens->data[curr_index + 1].type == TokenType_ident &&
    tokens->data[curr_index + 2].type == TokenType_equ){
        curr_index += 3;
        *index = curr_index;
        node_stmt *stmt_var;
        HEAP_ALLOC(heap, node_stmt, stmt_var);
        stmt_var->type = StmtType_var;
        node_expr *expr = parse_expr(tokens, index, 0);
        if(expr == NULL){
            fprintf(stderr, "Unable to parse expression\n");
            exit(EXIT_FAILURE);
        }
        stmt_var->expr = expr;
        curr_index = *index;
        if(tokens->data[curr_index].type != TokenType_semi){
            fprintf(stderr, "Expected ';'\n");
            exit(EXIT_FAILURE);
        }
        return stmt_var;
    }

    *index = curr_index;
    return NULL;
}

node_prog parse_prog(dynlist_token *tokens){
    node_prog prog;
    dynlist_init(prog, node_stmt*);
    for(u32 i = 0; i < tokens->size; i++){
        node_stmt *stmt = parse_stmt(tokens, &i);
        if(stmt != NULL){
            dynlist_push(prog, stmt);
        }
        else{
            fprintf(stderr, "Invalid statement\n");
            exit(EXIT_FAILURE);
        }
    }

    return prog;
}