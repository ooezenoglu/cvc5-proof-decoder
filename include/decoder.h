#ifndef decoder
#define decoder

#include "helpers.h"

void formatProof();
char* generateTypeVar();
bool replaceAll(char* str, char* pattern, char* replacement);
void preparse();
void refactor();
void parse();
void decode();

extern struct node *typeList;
extern struct node *varList;
extern struct type *types;
extern struct var *vars;
extern struct args *args;
#endif