#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "generate.h"
#include "../parse/parse.h"
#include "../token/token.h"
#include "../typedefs.h"
#include "../alloc.h"

size_t stack_size = 0;

typedef struct{
    size_t stack_loc;
} stack_var;


void push(FILE *fptr, const char *reg){
    fprintf(fptr, "    push %s\n", reg);
    stack_size++;
}

void pop(FILE *fptr, const char *reg){
    fprintf(fptr, "    pop %s\n", reg);
    stack_size--;
}

void generate(dynlist_stmt *prog)
{
    // assert(false && "Code generation not implemented\n");
    // Generate and open the file
    FILE *fptr;
    fptr = fopen("./build/out.S", "w");

    // Put code into the file
    fprintf(fptr, "global _start\n_start:\n");

    for(u32 i = 0; i < prog->size; i++){
        node_stmt *n = prog->data[i];
        printf("Statement #%i\n", i);
        if(n->type == StmtType_exit){
            printf("\texit statement\n");
            if(n->expr->type == ExprType_Add){
                printf("adding in an exit statement\n");
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
                    pop(fptr, "rdi");
                    fprintf(fptr, "    syscall\n");
                }
            }
        }
        else if(n->type == StmtType_var){
            printf("\tvariable statement\n");
            if(n->expr->type == ExprType_Add){
                printf("adding in a variable statement\n");
                // fprintf(fptr, "    ; %s\n", n->expr->);
                fprintf(fptr, "    mov rax, %s\n", n->expr->binary.left->term->int_lit.val);
                fprintf(fptr, "    mov rdi, %s\n", n->expr->binary.right->term->int_lit.val);
                fprintf(fptr, "    add rax, rdi\n");
                push(fptr, "rax");
            }
            else{
                fprintf(fptr, "    mov rax, %s\n", n->expr->term->int_lit.val);
                push(fptr, "rax");
            }
        }
    }

    fprintf(fptr, "    mov rax, 60\n    mov rdi, 0\n");
    fprintf(fptr, "    syscall\n");

    // Close the file
    fclose(fptr);
}