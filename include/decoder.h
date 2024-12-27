#ifndef decoder
#define decoder

#include "helpers.h"

#define NOTTRUE "not true"
#define FALSE "false"
#define NOTFALSE "not false"
#define TRUE "true"
#define NOTNOT "not not"
#define NOTFORALL "not forall"
#define NOTEXISTS "not exists"
#define EXISTSNOT "exists not"
#define FORALLNOT "forall not"
#define IMPL "=>"

char* generateTypeVar();
bool replaceAll(char* str, char* pattern, char* replacement);
bool applyDeMorgansLaw(char* str);
bool simplifyImplication(char* str);
bool simplifyNotExists(char* str);
bool simplifyNotForall(char* str);
bool simplifyDoubleNeg(char* str);
bool simplifyNotFalse(char* str);
bool simplifyNotTrue(char* str);
void preparse();
void refactor();
void decode();

extern struct node *typeList;
extern struct node *varList;
extern struct type *types;
extern struct var *vars;
extern struct args *args;
#endif