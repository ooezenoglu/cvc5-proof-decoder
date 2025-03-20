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
#include <ctype.h>
#include <wchar.h>
#include <stdlib.h>
#include <locale.h>
#include <wctype.h>
#include "uthash.h"
#include "decoder.h"

#define BUFFER_SIZE 256
#define SMT2 "smt2"
#define P "p"
#define NUM_EXTENSIONS 2

struct node {
    void* structure;
    struct node *next;
};

struct var {
    char name[BUFFER_SIZE];
    char type[BUFFER_SIZE];
    struct var *next;
};

struct type {
    char original[BUFFER_SIZE];
    char replacement[BUFFER_SIZE];
    int arity;
    struct type *next;
};

struct args {
    char p_file[BUFFER_SIZE];
    char smt2_file[BUFFER_SIZE];
    char proof_raw[BUFFER_SIZE];
    char proof_ref[BUFFER_SIZE];
    char proof_par[BUFFER_SIZE];
    char proof_sim[BUFFER_SIZE];
    char proof_for[BUFFER_SIZE];
    bool parse;
    bool run;
    bool decode;
    char parserPath[BUFFER_SIZE];
    char cvc5Path[BUFFER_SIZE];
    char result[BUFFER_SIZE];
};

struct style {
    char orig[BUFFER_SIZE];
    char simplified[BUFFER_SIZE];
};

struct line {
    char type[BUFFER_SIZE];
    char body[BUFFER_SIZE];
    char resolved_body[BUFFER_SIZE];
    char rule[BUFFER_SIZE];
    struct style prems;
    struct style args;
    char note[BUFFER_SIZE];
};

struct hashTable {
    char tag[BUFFER_SIZE]; // key
    struct line line; // value
    UT_hash_handle hh; // uthash structure
};

struct dict {
    char key[BUFFER_SIZE];
    char val[BUFFER_SIZE];
    UT_hash_handle hh;
};

// linked list functions
void printVarList(struct node *head);
void printTypeList(struct node *head);
struct node* addNode(void* structure, int size);
void push(struct node** head, void* structure, int size);

void adjustBrackets(char *str);
void removeDuplicateBrackets(char *str);
void cleanString(char *str);
char* generateTypeVar();
void printArgsStruct();
void printHashTable();
void trimWhitespaces(char *str);
bool replaceAll(char* str, char* pattern, char* replacement);
bool contains(const char *str, const char *substr);
bool startsWith(char *str, char *comp);
char* stringCopy(char *src);
bool isEqual(char *str1, char *str2);
void openFile(char *output, char *input, char *suffix);
char *removeFileExtension(char *file);
int getIndexOfFileExtension(char *file);
char *getFileExtension(char *file);
void setExecPermissions(char *path);
void errNdie(char *msg);
void extractCommandLineArgs(int argc, char *argv[]);

extern struct dict *symbs;
extern struct hashTable *table;
extern struct hashTable *entry;
extern struct node *typeList;
extern struct node *varList;
extern struct type *types;
extern struct var *vars;
extern struct args *args;
#endif