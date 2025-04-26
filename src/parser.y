%{
#include "../include/ast.h"
#include "../include/parse_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// global variable to store simplified AST
AST *result_ast = NULL;
%}

// ast header needed before parser compilation
%code requires {
    #include "../include/ast.h"
    #include "../include/parse_util.h"
}

// definitions of semantics
%union {
    int intval;
    char* str;
    AST* ast;
}

// token declarations
%token <str> VAR NUMBER
%token LPAREN RPAREN
%token NOT AND OR FORALL EXISTS IMP EQ TRUE FALSE

// nonterminals with AST value
%type <ast> expr non_empty_arg_list expr_list

%%

// start rule: if error is encountered, output error message, set result_ast to NULL, clear the error token, accept the parse
input:
    expr { result_ast = simplifyAST($1); }
  | error { yyerror("Syntax error in input"); result_ast = NULL; yyclearin; YYACCEPT; }
;

// Atomic expressions and groupings
// A grouping is created as NODE_GROUP to preserve explicitly existing brackets.
expr:
      VAR                                            { $$ = new_node(NODE_VAR, NULL, NULL, $1); }
    | NUMBER                                         { $$ = new_node(NODE_NUMBER, NULL, NULL, $1); }
    | TRUE                                           { $$ = new_node(NODE_TRUE, NULL, NULL, "true"); }
    | FALSE                                          { $$ = new_node(NODE_FALSE, NULL, NULL, "false"); }
    | LPAREN expr RPAREN                             { $$ = new_node(NODE_GROUP, $2, NULL, NULL); }
    | LPAREN expr_list RPAREN                        { $$ = new_node(NODE_GROUP, $2, NULL, NULL); }
    // operations in prefix notation
    | LPAREN NOT expr RPAREN                         { $$ = new_node(NODE_NOT, $3, NULL, NULL); }
    | LPAREN AND expr expr RPAREN                    { $$ = new_node(NODE_AND, $3, $4, NULL); }
    | LPAREN OR expr expr RPAREN                     { $$ = new_node(NODE_OR, $3, $4, NULL); }
    | LPAREN IMP expr expr RPAREN                    { $$ = new_node(NODE_IMP, $3, $4, NULL); }
    | LPAREN EQ expr expr RPAREN                     { $$ = new_node(NODE_EQ, $3, $4, NULL); }
    // quantifier patterns
    | LPAREN FORALL LPAREN VAR RPAREN expr RPAREN     { $$ = new_node(NODE_FORALL, $6, NULL, $4); }
    | LPAREN EXISTS LPAREN VAR RPAREN expr RPAREN     { $$ = new_node(NODE_EXISTS, $6, NULL, $4); }
    // function applications without arguments
    | LPAREN VAR RPAREN                              { $$ = new_func_node($2, NULL); }
    // function applications with at least one argument
    | LPAREN VAR non_empty_arg_list RPAREN           { $$ = new_func_node($2, $3); }
    ;

// non_empty_arg_list remains unchanged for function arguments
non_empty_arg_list:
      expr                      { $$ = new_node(NODE_ARG_LIST, $1, NULL, NULL); }
    | expr non_empty_arg_list  { $$ = new_node(NODE_ARG_LIST, $1, $2, NULL); }
    ;

// expr_list for grouping multiple expressions (used only for grouping, not function arguments)
expr_list:
      expr                      { $$ = $1; }
    | expr expr_list            { $$ = new_node(NODE_SEQ, $1, $2, NULL); }
    ;

%%

AST* new_node(NodeType type, AST* left, AST* right, char* var) {
    AST* node = (AST*) malloc(sizeof(AST));
    node->type = type;
    node->left = left;
    node->right = right;
    if (var != NULL) {
        node->var = strdup(var);
    } else {
        node->var = NULL;
    }
    return node;
}

AST* new_func_node(char *func_name, AST *arg_list) {
    // creates a NODE_FUNC node; the function name is stored in var,
    // the arguments are linked in the arg_list (via NODE_ARG_LIST nodes).
    AST *node = new_node(NODE_FUNC, arg_list, NULL, func_name);
    return node;
}

// recursively simplify an AST
AST* simplifyAST(AST* node) {
    if (!node) return NULL;
    node->left = simplifyAST(node->left);
    node->right = simplifyAST(node->right);
    bool changed;
    do {
        changed = false;
        changed |= implication(node);
        changed |= doubleNeg(node);
        changed |= notForall(node);
        changed |= notExists(node);
        changed |= deMorg(node);
        node->left = simplifyAST(node->left);
        node->right = simplifyAST(node->right);
    } while (changed);
    return node;
}

// A => B becomes (or (not A) B)
bool implication(AST* node) {
    if (!node) return false;
    if (node->type == NODE_IMP) {
         AST* notA = new_node(NODE_NOT, node->left, NULL, NULL);
         AST* replacement = new_node(NODE_OR, notA, node->right, NULL);
         node->type = replacement->type;
         node->left = replacement->left;
         node->right = replacement->right;
         node->var = replacement->var;
         return true;
    }
    return false;
}

// not (not X) becomes X
bool doubleNeg(AST* node) {
    if (!node) return false;
    if (node->type == NODE_NOT && node->left && node->left->type == NODE_NOT) {
         AST* child = node->left->left;
         if (child) {
             node->type = child->type;
             node->left = child->left;
             node->right = child->right;
             node->var = child->var;
             return true;
         }
    }
    return false;
}

// not (forall (x) X) becomes (exists (x) (not X))
bool notForall(AST* node) {
    if (!node) return false;
    if (node->type == NODE_NOT && node->left && node->left->type == NODE_FORALL) {
         AST* quant = node->left;
         AST* new_body = new_node(NODE_NOT, quant->left, NULL, NULL);
         node->type = NODE_EXISTS;
         node->left = new_body;
         node->right = NULL;
         node->var = quant->var;
         return true;
    }
    return false;
}

// not (exists (x) X) becomes (forall (x) (not X))
bool notExists(AST* node) {
    if (!node) return false;
    if (node->type == NODE_NOT && node->left && node->left->type == NODE_EXISTS) {
         AST* quant = node->left;
         AST* new_body = new_node(NODE_NOT, quant->left, NULL, NULL);
         node->type = NODE_FORALL;
         node->left = new_body;
         node->right = NULL;
         node->var = quant->var;
         return true;
    }
    return false;
}

// apply De Morgan's law
bool deMorg(AST* node) {
    if (!node) return false;
    if (node->type == NODE_NOT && node->left) {
         AST* child = node->left;
         if (child->type == NODE_AND) {
             AST* new_left = new_node(NODE_NOT, child->left, NULL, NULL);
             AST* new_right = new_node(NODE_NOT, child->right, NULL, NULL);
             node->type = NODE_OR;
             node->left = new_left;
             node->right = new_right;
             node->var = NULL;
             return true;
         } else if (child->type == NODE_OR) {
             AST* new_left = new_node(NODE_NOT, child->left, NULL, NULL);
             AST* new_right = new_node(NODE_NOT, child->right, NULL, NULL);
             node->type = NODE_AND;
             node->left = new_left;
             node->right = new_right;
             node->var = NULL;
             return true;
         } else if (child->type == NODE_IMP) {
             AST* new_right = new_node(NODE_NOT, child->right, NULL, NULL);
             node->type = NODE_AND;
             node->left = child->left;
             node->right = new_right;
             node->var = NULL;
             return true;
         }
    }
    return false;
}

void ast_to_string(AST *node, char *buffer, size_t bufsize) {
    if (!node) {
        snprintf(buffer, bufsize, "null");
        return;
    }
    char temp[1024] = {0};
    switch(node->type) {
        case NODE_VAR:
            snprintf(buffer, bufsize, "%s", node->var);
            break;
        case NODE_TRUE:
        case NODE_FALSE:
            snprintf(buffer, bufsize, "%s", node->var);
            break;
        case NODE_NOT:
            snprintf(buffer, bufsize, "(not ");
            ast_to_string(node->left, temp, sizeof(temp));
            strncat(buffer, temp, bufsize - strlen(buffer) - 1);
            strncat(buffer, ")", bufsize - strlen(buffer) - 1);
            break;
        case NODE_AND:
            snprintf(buffer, bufsize, "(and ");
            ast_to_string(node->left, temp, sizeof(temp));
            strncat(buffer, temp, bufsize - strlen(buffer) - 1);
            strncat(buffer, " ", bufsize - strlen(buffer) - 1);
            ast_to_string(node->right, temp, sizeof(temp));
            strncat(buffer, temp, bufsize - strlen(buffer) - 1);
            strncat(buffer, ")", bufsize - strlen(buffer) - 1);
            break;
        case NODE_OR:
            snprintf(buffer, bufsize, "(or ");
            ast_to_string(node->left, temp, sizeof(temp));
            strncat(buffer, temp, bufsize - strlen(buffer) - 1);
            strncat(buffer, " ", bufsize - strlen(buffer) - 1);
            ast_to_string(node->right, temp, sizeof(temp));
            strncat(buffer, temp, bufsize - strlen(buffer) - 1);
            strncat(buffer, ")", bufsize - strlen(buffer) - 1);
            break;
        case NODE_IMP:
            snprintf(buffer, bufsize, "(=> ");
            ast_to_string(node->left, temp, sizeof(temp));
            strncat(buffer, temp, bufsize - strlen(buffer) - 1);
            strncat(buffer, " ", bufsize - strlen(buffer) - 1);
            ast_to_string(node->right, temp, sizeof(temp));
            strncat(buffer, temp, bufsize - strlen(buffer) - 1);
            strncat(buffer, ")", bufsize - strlen(buffer) - 1);
            break;
        case NODE_EQ:
            snprintf(buffer, bufsize, "(= ");
            ast_to_string(node->left, temp, sizeof(temp));
            strncat(buffer, temp, bufsize - strlen(buffer) - 1);
            strncat(buffer, " ", bufsize - strlen(buffer) - 1);
            ast_to_string(node->right, temp, sizeof(temp));
            strncat(buffer, temp, bufsize - strlen(buffer) - 1);
            strncat(buffer, ")", bufsize - strlen(buffer) - 1);
            break;
        case NODE_FORALL:
            snprintf(buffer, bufsize, "(forall (%s) ", node->var);
            ast_to_string(node->left, temp, sizeof(temp));
            strncat(buffer, temp, bufsize - strlen(buffer) - 1);
            strncat(buffer, ")", bufsize - strlen(buffer) - 1);
            break;
        case NODE_EXISTS:
            snprintf(buffer, bufsize, "(exists (%s) ", node->var);
            ast_to_string(node->left, temp, sizeof(temp));
            strncat(buffer, temp, bufsize - strlen(buffer) - 1);
            strncat(buffer, ")", bufsize - strlen(buffer) - 1);
            break;
        case NODE_GROUP: {
            char inner[1024] = {0};
            ast_to_string(node->left, inner, sizeof(inner));
            snprintf(buffer, bufsize, "(%s)", inner);
            break;
        }
        case NODE_FUNC:
            snprintf(buffer, bufsize, "(%s", node->var);
            if (node->left) {
                strncat(buffer, " ", bufsize - strlen(buffer) - 1);
                AST *arg = node->left;
                while(arg) {
                    ast_to_string(arg->left, temp, sizeof(temp));
                    strncat(buffer, temp, bufsize - strlen(buffer) - 1);
                    if(arg->right)
                        strncat(buffer, " ", bufsize - strlen(buffer) - 1);
                    arg = arg->right;
                }
            }
            strncat(buffer, ")", bufsize - strlen(buffer) - 1);
            break;
        case NODE_ARG_LIST:
            snprintf(buffer, bufsize, "ARG_LIST");
            break;
        case NODE_NUMBER:
            snprintf(buffer, bufsize, "%s", node->var);
            break;
        case NODE_SEQ: {
            char left_str[1024] = {0};
            char right_str[1024] = {0};
            ast_to_string(node->left, left_str, sizeof(left_str));
            ast_to_string(node->right, right_str, sizeof(right_str));
            snprintf(buffer, bufsize, "%s %s", left_str, right_str);
            break;
        }
        default:
            snprintf(buffer, bufsize, "UNKNOWN");
            break;
    }
}

void yyerror(const char* s) {
    // silence error message
}
