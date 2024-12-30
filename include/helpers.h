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
};

// linked list functions
void printVarList(struct node *head);
void printTypeList(struct node *head);
struct node* addNode(void* structure, int size);
void push(struct node** head, void* structure, int size);

bool contains(const char *str, const char *substr);
void setExecPermissions(char *path);
void printArgsStruct();
bool startsWith(char *str, char *comp);
char* stringCopy(char *src);
bool isEqual(char *str1, char *str2);
char *removeFileExtension(char *file);
int getIndexOfFileExtension(char *file);
char *getFileExtension(char *file);
void errNdie(char *msg);

extern struct node *typeList;
extern struct node *varList;
extern struct type *types;
extern struct var *vars;
extern struct args *args;
#endif