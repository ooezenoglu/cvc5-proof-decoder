#ifndef encoder
#define encoder

#include "helpers.h"

void runCvc5Parser();
void extractCommandLineArgs(int argc, char *argv[]);

extern struct args *args;
#endif