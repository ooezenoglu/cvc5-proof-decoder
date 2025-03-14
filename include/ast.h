#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { 
    NODE_VAR, 
    NODE_NOT, 
    NODE_AND, 
    NODE_OR, 
    NODE_IMP,   
    NODE_EQ, 
    NODE_FORALL, 
    NODE_EXISTS,
    NODE_TRUE,
    NODE_FALSE,
    NODE_GROUP,   // group to maintain brackets
    NODE_FUNC,    // function application
    NODE_ARG_LIST // argument list for function application
} NodeType;

typedef struct AST {
    NodeType type;
    char *var;
    struct AST *left;
    struct AST *right;
} AST;

#endif
