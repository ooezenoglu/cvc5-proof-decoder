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
#include "uthash.h"

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

struct file {
    char file[BUFFER_SIZE];
    char name[BUFFER_SIZE];
    char extension[BUFFER_SIZE];
};

struct input {
    struct file p;
    struct file smt2;
};

struct output {
    struct file raw;
    struct file preparsed;
    struct file refactored;
    struct file parsed;
    struct file formatted;
};

struct args {
    struct input in;
    struct output out;
    bool parse;
    bool simplify;
    bool run;
    bool decode;
    char parserPath[BUFFER_SIZE];
    char cvc5Path[BUFFER_SIZE];
    char result[BUFFER_SIZE];
};

struct line {
    char type[BUFFER_SIZE];
    char rest[BUFFER_SIZE];
    char rule[BUFFER_SIZE];
    char prems[BUFFER_SIZE];
    char args[BUFFER_SIZE];
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

bool contains(const char *str, const char *substr);
void setExecPermissions(char *path);
void printArgsStruct();
void printHashTable();
void addSymbol(char *key, char *value);
void trimWhitespaces(char *str);
bool startsWith(char *str, char *comp);
char* stringCopy(char *src);
bool isEqual(char *str1, char *str2);
char *removeFileExtension(char *file);
int getIndexOfFileExtension(char *file);
char *getFileExtension(char *file);
void errNdie(char *msg);

extern struct dict *symbols;
extern struct hashTable *table;
extern struct hashTable *entry;
extern struct node *typeList;
extern struct node *varList;
extern struct type *types;
extern struct var *vars;
extern struct args *args;
#endif