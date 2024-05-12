#ifndef EB_PARSE_H
#define EB_PARSE_H

#include "../token/token.h"
#include "../typedefs.h"
#include "../alloc.h"

typedef enum{
    TermType_NULL,
    TermType_int_lit,
    TermType_ident,
    TermType_paren,
} node_term_type;

typedef enum {
    ExprType_NULL,
    ExprType_Error,
    ExprType_Number,
    ExprType_Positive,
    ExprType_Negative,
    ExprType_Add,
    ExprType_Sub,
    ExprType_Mult,
    ExprType_Div,
    ExprType_Term,
} node_expr_type;

typedef enum{
    StmtType_NULL,
    StmtType_exit,
    StmtType_var,
    StmtType_expr,
} node_stmt_type;

typedef struct node_stmt node_stmt;
typedef struct node_expr node_expr;
typedef struct node_term node_term;
struct node_expr{
    node_expr_type type;

    union{
        node_term *term;
        struct { node_expr *operand; } unary;
        struct { node_expr *left; node_expr *right; } binary;
    };
};

struct node_term{
    node_term_type type;
    Token int_lit;
    Token ident;
    node_expr *expr;
};

struct node_stmt{
    node_stmt_type type;
    node_expr *expr;
};

typedef dynlist(node_stmt*) dynlist_stmt;
typedef dynlist_stmt node_prog;

node_expr *parse_expr(dynlist_token *tokens, u32 *index, int min_prec);

node_stmt *parse_stmt(dynlist_token *tokens, u32 *curr_index);

node_prog parse_prog(dynlist_token *tokens);

// Creating the heap as a global variable in so each
// function can use the heap
// The heap will have a size of 4 megabytes
extern HEAP heap;

#endif