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
    char *ident;
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

void getVar(FILE *fptr, const char *reg, size_t offset){
    fprintf(fptr, "    lea rsp, [rsp + %li]\n", 8*offset);
    fprintf(fptr, "    pop %s\n", reg);
    stack_size--;
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
        printf("Statement #%i\n", i+1);
        if(n->type == StmtType_exit){
            printf("\texit statement\n");
            if(n->expr->type == ExprType_Add){
                fprintf(fptr, "    mov rax, %s\n", n->expr->binary.left->term->int_lit.val);
                fprintf(fptr, "    mov rdi, %s\n", n->expr->binary.right->term->int_lit.val);
                fprintf(fptr, "    add rax, rdi\n");
                push(fptr, "rax");
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
                    getVar(fptr, "rdi", 1);
                    fprintf(fptr, "    syscall\n");
                }
            }
        }
        else if(n->type == StmtType_var){
            printf("\tvariable statement\n");
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

                // asm for this
                fprintf(fptr, "    ; %s\n", n->expr->term->ident.val);
                fprintf(fptr, "    mov rax, %s\n", n->expr->binary.left->term->int_lit.val);
                fprintf(fptr, "    mov rdi, %s\n", n->expr->binary.right->term->int_lit.val);
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

                // TODO : dynlist_push(stack_vars, var) NOT WORKING WTF
                // this is done down here because it wasn't working when it was above
                // var.ident = n->expr->term->ident.val;
                // dynlist_push(stack_vars, var);
            }
        }
    }

    fprintf(fptr, "    mov rax, 60\n    mov rdi, 0\n");
    fprintf(fptr, "    syscall\n");

    // Close the file
    fclose(fptr);

    for(size_t i = 0; i < stack_vars.size; i++){
        free(stack_vars.data[i].ident);
    }
    dynlist_free(stack_vars);
}