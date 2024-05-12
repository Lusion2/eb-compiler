#ifndef EB_GENERATOR_H
#define EB_GENERATOR_H

#include "../token/token.h"
#include "../parse/parse.h"
#include "../typedefs.h"
#include "../alloc.h"

/*
 * Takes tokens from tokenize() (soon to be from parsed but that's for later) and generates assembly code from it
 */
void generate(dynlist_stmt *prog);

#endif