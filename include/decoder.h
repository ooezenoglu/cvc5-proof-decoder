#ifndef decoder
#define decoder

#include "helpers.h"

void extractComponents(char *line, char *tag, char *type, char *body);
int extractSubcomponents(char *type, char *body, char *args, char *prems, char *note, char *rule);
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