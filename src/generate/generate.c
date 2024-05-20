#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "generate.h"
#include "../parse/parse.h"
#include "../token/token.h"
#include "../typedefs.h"
#include "../alloc.h"

typedef struct{
    size_t stack_loc;
    const char *ident;
} stack_var;

typedef dynlist(stack_var) dynlist_stack_var;
size_t stack_size = 0;
dynlist_stack_var stack_vars;


void push(FILE *fptr, const char *reg){
    fprintf(fptr, "    push %s\n", reg);
    stack_size++;
}

void pop(FILE *fptr, const char *reg){
    fprintf(fptr, "    pop %s\n", reg);
    stack_size--;
}

void get_var(FILE *fptr, const char *reg, const char *ident, bool use_offset_in, size_t offset_in){
    size_t offset = 0;
    if(ident != NULL && !use_offset_in){
        bool initialized_var = false;
        for(size_t i = 0; i < stack_vars.size; i++){
            if(strcmp(stack_vars.data[i].ident, ident) == 0){
                offset = (stack_size - stack_vars.data[i].stack_loc - 1) * 8;
                if((int)offset < 0){
                    offset = 0;
                }
                initialized_var = true;
                break;
            }
        }
        if(!initialized_var){
            fprintf(stderr, "ERROR: Variable, %s, is not initialized\n", ident);
            system("rm build/out.S");
            exit(EXIT_FAILURE);
        }
    }
    else if(use_offset_in){
        offset = offset_in * 8;
    }
    printf("offset: %li\n", offset);

    char offset_str[64];
    strcpy(offset_str, "QWORD [rsp + ");
    char *offset_i = (char*)malloc(sizeof(char)*32);
    snprintf(offset_i, 32, "%ld", offset);
    strcat(offset_str, offset_i);
    strcat(offset_str, "]");
    printf("offset in str: %s\n", offset_str);
    

    push(fptr, offset_str);
    pop(fptr, reg);

    free(offset_i);
}

void expr_add(FILE *fptr, node_stmt *n){
    if(n->expr->binary.left->term->type == TermType_int_lit){
        fprintf(fptr, "    mov rax, %s\n", n->expr->binary.left->term->int_lit.val);
    }
    else if(n->expr->binary.left->term->type == TermType_ident){
        for(size_t i = 0; i < stack_vars.size; i++){
            if(strcmp(n->expr->binary.left->term->ident.val, stack_vars.data[i].ident) == 0){
                get_var(fptr, "rax", stack_vars.data[i].ident, false, 0);
                break;
            }
        }
    }
    
    if(n->expr->binary.right->term->type == TermType_int_lit){
        fprintf(fptr, "    mov rdi, %s\n", n->expr->binary.right->term->int_lit.val);
    }
    else if(n->expr->binary.right->term->type == TermType_ident){
        for(size_t i = 0; i < stack_vars.size; i++){
            if(strcmp(n->expr->binary.right->term->ident.val, stack_vars.data[i].ident) == 0){
                get_var(fptr, "rdi", stack_vars.data[i].ident, false, 0);
                break;
            }
        }
    }
    fprintf(fptr, "    add rax, rdi\n");
    push(fptr, "rax");
}

void generate_expr(FILE *fptr, node_expr *expr){
    if (expr->type == ExprType_Add){
        // asm for this
        if (expr->binary.left->type == ExprType_Term){
            if(expr->binary.left->term->type == TermType_ident){
                get_var(fptr, "rax", expr->binary.left->term->ident.val, false, 0);
            } else{
                fprintf(fptr, "    mov rax, %s\n", expr->binary.left->term->int_lit.val);
            }
        }
        if (expr->binary.right->type == ExprType_Term){
            if(expr->binary.right->term->type == TermType_ident){
                get_var(fptr, "rdi", expr->binary.right->term->ident.val, false, 0);
            } else{
                fprintf(fptr, "    mov rdi, %s\n", expr->binary.right->term->int_lit.val);
            }
        }
        else{
            push(fptr, "rax");
            generate_expr(fptr, expr->binary.right);
            // pop(fptr, "rax");
            get_var(fptr, "rdi", NULL, true, 1);
            fprintf(fptr, "    add rax, rdi\n");

            push(fptr, "rax");
            return;
        }

        fprintf(fptr, "    add rax, rdi\n");
        push(fptr, "rax");
    }
    else if(expr->type == ExprType_Term){
        printf("%s\n", expr->term->ident.val);
        if(expr->term->type == TermType_ident){
            get_var(fptr, "rax", expr->term->ident.val, false, 0);
            push(fptr, "rax");
        }
        else if(expr->term->type == TermType_int_lit){
            fprintf(fptr, "    mov rax, %s\n", expr->term->int_lit.val);
            push(fptr, "rax");
        }
    }
}

void generate(dynlist_stmt *prog)
{
    // init the stack_var dynlist
    dynlist_init(stack_vars, stack_var);

    // Generate and open the file
    FILE *fptr;
    fptr = fopen("./build/out.S", "w");

    // Put code into the file
    fprintf(fptr, "global _start\n_start:\n");

    for(u32 i = 0; i < prog->size; i++){
        node_stmt *n = prog->data[i];
        if(n->type == StmtType_exit){
            if(n->expr->type == ExprType_Add){
                expr_add(fptr, n);
                fprintf(fptr, "    mov rax, 60\n");
                pop(fptr, "rdi");
                fprintf(fptr, "    syscall\n");
            }
            else if(n->expr->type == ExprType_Term){
                if(n->expr->term->type == TermType_int_lit){
                    fprintf(fptr, "    mov rax, 60\n    mov rdi, %s\n", n->expr->term->int_lit.val);
                    fprintf(fptr, "    syscall\n");
                }
                else if(n->expr->term->type == TermType_ident){
                    fprintf(fptr, "    mov rax, 60\n");
                    get_var(fptr, "rdi", n->expr->term->ident.val, false, 0);
                    fprintf(fptr, "    syscall\n");
                }
            }
        }
        else if(n->type == StmtType_var){
            // Creating a new variable
                
            // saving the variable in our stack struct
            stack_var var = {.stack_loc = stack_size};
            for(size_t i = 0; i < stack_vars.size; i++){
                if(strcmp(n->expr->term->ident.val, stack_vars.data[i].ident) == 0){
                    fprintf(stderr, "ERROR Redefinition of variable: %s\n", n->expr->term->ident.val);
                    exit(EXIT_FAILURE);
                }
            }
            var.ident = n->expr->term->ident.val;
            dynlist_push(stack_vars, var);

            fprintf(fptr, "    ; %s\n", n->expr->term->ident.val);
            generate_expr(fptr, n->expr);


            // this is done down here because it wasn't working when it was above
                

                
                
        }
    }

    fprintf(fptr, "    mov rax, 60\n    mov rdi, 0\n");
    fprintf(fptr, "    syscall\n");

    // Close the file
    fclose(fptr);

    for(size_t i = 0; i < stack_vars.size; i++){
        // free(stack_vars.data[i].ident);
    }
    dynlist_free(stack_vars);
}