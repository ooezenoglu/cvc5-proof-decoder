#ifndef PARSE_UTIL_H
#define PARSE_UTIL_H

#include "../include/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int yyparse(void);
extern void yy_scan_string(const char*);

// global var that stores simplified AST
extern AST *result_ast;

void ast_to_string(AST *node, char *buffer, size_t bufsize);

#ifdef __cplusplus
}
#endif

#endif
