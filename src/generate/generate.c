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

void get_var(FILE *fptr, const char *reg, const char *ident){
    size_t offset = 0;
    bool initialized_var = false;
    for(size_t i = 0; i < stack_vars.size; i++){
        if(strcmp(stack_vars.data[i].ident, ident) == 0){
            offset = (stack_size - stack_vars.data[i].stack_loc - 1) * 8;
            initialized_var = true;
            break;
        }
    }
    if(!initialized_var){
        fprintf(stderr, "ERROR: Variable, %s, is not initialized\n", ident);
        system("rm build/out.S");
        exit(EXIT_FAILURE);
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
                get_var(fptr, "rax", stack_vars.data[i].ident);
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
                get_var(fptr, "rdi", stack_vars.data[i].ident);
                break;
            }
        }
    }
    fprintf(fptr, "    add rax, rdi\n");
    push(fptr, "rax");
}

void generate_expr(FILE *fptr, node_stmt *n){
    if (n->expr->type == ExprType_Add){
        // asm for this
        fprintf(fptr, "    ; %s\n", n->expr->term->ident.val);
        if (n->expr->binary.left->type == ExprType_Term){
            if(n->expr->binary.left->term->type == TermType_ident){
                get_var(fptr, "rax", n->expr->binary.left->term->ident.val);
            } else{
                fprintf(fptr, "    mov rax, %s\n", n->expr->binary.left->term->int_lit.val);
            }
        }
        if (n->expr->binary.right->type == ExprType_Term){
            if(n->expr->binary.right->term->type == TermType_ident){
                get_var(fptr, "rdi", n->expr->binary.right->term->ident.val);
            } else{
                fprintf(fptr, "    mov rdi, %s\n", n->expr->binary.right->term->int_lit.val);
            }
        }
    }
}

void generate(dynlist_stmt *prog)
{
    // init the stack_var dynlist
    dynlist_init(stack_vars, stack_var);
    for(size_t i = 0; i < stack_vars.size; i++){
        stack_vars.data[i].ident = (char*)malloc(sizeof(char));
    }

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
                    get_var(fptr, "rdi", n->expr->term->ident.val);
                    fprintf(fptr, "    syscall\n");
                }
            }
        }
        else if(n->type == StmtType_var){
            if(n->expr->type == ExprType_Add){
                // Creating a new variable
                
                // saving the variable in our stack struct
                stack_var var = {.stack_loc = stack_size};
                for(size_t i = 0; i < stack_vars.size; i++){
                    if(strcmp(n->expr->term->ident.val, stack_vars.data[i].ident) == 0){
                        fprintf(stderr, "ERROR Redefinition of variable: %s\n", n->expr->term->ident.val);
                        exit(EXIT_FAILURE);
                    }
                }

                generate_expr(fptr, n);
                
                fprintf(fptr, "    add rax, rdi\n");
                push(fptr, "rax");

                // this is done down here because it wasn't working when it was above
                var.ident = n->expr->term->ident.val;
                dynlist_push(stack_vars, var);
            
            }
            else{
                // saving the variable in our stack struct
                stack_var var = {.stack_loc = stack_size};
                
                for(size_t i = 0; i < stack_vars.size; i++){
                    if(strcmp(n->expr->term->ident.val, stack_vars.data[i].ident) == 0){
                        fprintf(stderr, "ERROR Redefinition of variable: %s\n", n->expr->term->ident.val);
                        exit(EXIT_FAILURE);
                    }
                }  

                fprintf(fptr, "    ; %s\n", n->expr->term->ident.val);
                fprintf(fptr, "    mov rax, %s\n", n->expr->term->int_lit.val);
                push(fptr, "rax");

                // this is done down here because it wasn't working when it was above
                var.ident = n->expr->term->ident.val;
                dynlist_push(stack_vars, var);
            }
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