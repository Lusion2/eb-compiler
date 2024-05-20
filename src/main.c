#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "dynlist.h"
#include "alloc.h"
#include "typedefs.h"
#include "./token/token.h"
#include "./parse/parse.h"
#include "./generate/generate.h"

HEAP heap;

/*
 * Takes the path to the file and an empty char * and returns
 * the text in the given file as a char *
 */
void load_code(const char *path, dynlist_char *code);

int main(int argc, char **argv)
{
    // Check if used correctly
    if (argc != 2){
        fprintf(stderr, "Incorrect Usage\nUsage: ./eb <input>");
        exit(EXIT_FAILURE);
    }

    // Initialize the heap
    HEAP_INIT(heap, 1024 * 2);

    // Initialize the char * code buffer and load it into memory
    dynlist_char code;
    load_code(argv[1], &code);
    if(code.size == 0){
        fprintf(stderr, "Code could not be loaded\n");
        exit(EXIT_FAILURE);
    }

    // Tokenize the code
    dynlist_token tokens;
    dynlist_init(tokens, Token);
    tokenize(&tokens, &code);

    // Free the code buffer after tokenization
    dynlist_free(code);

    dynlist_stmt prog = {0};
    prog = parse_prog(&tokens);

    generate(&prog);
    
    // Free the tokens after code gen
    for(size_t i = 0; i < tokens.size; i++){
        if (tokens.data[i].type == TokenType_int_lit ||
            tokens.data[i].type == TokenType_ident)
        {
            free(tokens.data[i].val);
        }
    }
    dynlist_free(tokens);

    // Free nodes after generating
    dynlist_free(prog);

    // Free the heap
    HEAP_FREE(heap);

    // Build the assembly file
    // system("nasm -felf64 ./build/out.S");
    // system("ld -o ./out ./build/out.o");

    return 0;
}

void load_code(const char *path, dynlist_char *buffer){
    // Open input file
    FILE *fptr;
    fptr = fopen(path, "r");

    // Check if file is not NULL
    if (fptr == NULL){
        fprintf(stderr, "Unable to open input file\n");
        fclose(fptr);
        exit(EXIT_FAILURE);
    }

    // Get the size of the file
    fseek(fptr, 0, SEEK_END);
    buffer->size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);

    // Allocate memory for entire content
    buffer->data = (char *)malloc((buffer->size + 1) * sizeof(char));
    if (buffer == NULL){
        fclose(fptr);
        fprintf(stderr, "Memory allocation failed");
        dynlist_free_ptr(buffer);
        exit(EXIT_FAILURE);
    }

    // Put the file contents into the char *
    fread(buffer->data, sizeof(char), buffer->size, fptr);

    // Close the file
    fclose(fptr);
}
