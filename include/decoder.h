#ifndef decoder
#define decoder

#include "helpers.h"

#define NOTNOT "not not"
#define NOTFORALL "not forall"
#define NOTEXISTS "not exists"
#define EXISTSNOT "exists not"
#define FORALLNOT "forall not"
#define IMPL "=>"

char* simplifyNotExists(char* str);
char* simplifyNotForall(char* str);
char* simplifyDoubleNeg(char* str);
void preparse();
void decode();

extern struct args *args;
#endif