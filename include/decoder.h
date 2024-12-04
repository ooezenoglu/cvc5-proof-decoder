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

void replaceAll(char* str, char* pattern, char* replacement);
void applyDeMorgansLaw(char* str);
void simplifyImplication(char* str);
void simplifyNotExists(char* str);
void simplifyNotForall(char* str);
void simplifyDoubleNeg(char* str);
void simplifyNotFalse(char* str);
void simplifyNotTrue(char* str);
void preparse();
void decode();

extern struct args *args;
#endif