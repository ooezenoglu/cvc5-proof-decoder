#ifndef helpers
#define helpers

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/stat.h>
#include <regex.h>

#define BUFFER_SIZE 256
#define SMT2 "smt2"
#define P "p"
#define NUM_EXTENSIONS 2

struct args {
    char file[BUFFER_SIZE];
    char fileName[BUFFER_SIZE];
    char fileExtension[BUFFER_SIZE];
    bool parse;
    bool simplify;
    bool decode;
    char parserPath[BUFFER_SIZE];
    char cvc5Path[BUFFER_SIZE];
    char proofFile[BUFFER_SIZE];
    char proofFileName[BUFFER_SIZE];
    char proofFileExtension[BUFFER_SIZE];
    char preparsedProofFile[BUFFER_SIZE];
    char refactoredProofFile[BUFFER_SIZE];
};

void setExecPermissions(char *path);
void printArgsStruct();
bool startsWith(char *str, char *comp);
char* stringCopy(char *src);
bool isEqual(char *str1, char *str2);
char *removeFileExtension(char *file);
int getIndexOfFileExtension(char *file);
char *getFileExtension(char *file);
void errNdie(char *msg);

extern struct args *args;
#endif