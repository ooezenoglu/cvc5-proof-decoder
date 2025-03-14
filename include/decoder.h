#ifndef decoder
#define decoder

#include "helpers.h"

void removeDuplicateBrackets(char *str);
void cleanString(char *str);
char* generateTypeVar();
bool replaceAll(char* str, char* pattern, char* replacement);
void refactor();
void parse();
void formatProof();
void decode();

extern struct node *typeList;
extern struct node *varList;
extern struct type *types;
extern struct var *vars;
extern struct args *args;
#endif