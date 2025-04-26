#ifndef PARSE_UTIL_H
#define PARSE_UTIL_H

#include "../include/ast.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int yyparse(void);

// internal flex type
struct yy_buffer_state;
typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char*);

// global var that stores simplified AST
extern AST *result_ast;

AST* new_node(NodeType type, AST *left, AST *right, char *var);
AST* new_func_node(char *func_name, AST *arg_list);
bool implication(AST* node);
bool doubleNeg(AST* node);
bool notForall(AST* node);
bool notExists(AST* node);
bool deMorg(AST* node);
AST* simplifyAST(AST *root);
void ast_to_string(AST *node, char *buffer, size_t bufsize);
void yyerror(const char *s);
int yylex(void);

#ifdef __cplusplus
}
#endif

#endif
