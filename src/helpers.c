#include "../include/helpers.h"

void generateOutputFile(char *output, char *input, char *suffix) {
    snprintf(output, BUFFER_SIZE, "%s%s", input, suffix);
}

bool contains(const char *str, const char *substr) {
    
    // check whether input strings are valid
    if (str == NULL || substr == NULL) {
        return false;
    }

    // check whether pointer to first occurence exists
    if (strstr(str, substr) != NULL) {
        return true;
    }

    return false;
}

void setExecPermissions(char *path) {
    /*
    S_IXUSR: User
    S_IXGRP: Group
    S_IXOTH: Others
    */
    if (chmod(path, S_IXUSR | S_IXGRP | S_IXOTH) != 0) {
        errNdie("Could not set execution permissions");
    }
}

void printArgsStruct() {

    printf("++++ INPUT ++++\n");
    printf("     P:\n");
    printf("        FILE: %s\n", args->in.p.file);
    printf("        NAME: %s\n", args->in.p.name);
    printf("        EXTENSION: %s\n", args->in.p.extension);    
    printf("     SMT2:\n");
    printf("        FILE: %s\n", args->in.smt2.file);
    printf("        NAME: %s\n", args->in.smt2.name);
    printf("        EXTENSION: %s\n", args->in.smt2.extension);
    printf("++++ OUTPUT ++++\n");
    printf("     RAW:\n");
    printf("        FILE: %s\n", args->out.raw.file);
    printf("        NAME: %s\n", args->out.raw.name);
    printf("        EXTENSION: %s\n", args->out.raw.extension);
    printf("     PREPARSED:\n");
    printf("        FILE: %s\n", args->out.preparsed.file);
    printf("        NAME: %s\n", args->out.preparsed.name);
    printf("        EXTENSION: %s\n", args->out.preparsed.extension);
    printf("     REFACTORED:\n");
    printf("        FILE: %s\n", args->out.refactored.file);
    printf("        NAME: %s\n", args->out.refactored.name);
    printf("        EXTENSION: %s\n", args->out.refactored.extension);
    printf("     PARSED:\n");
    printf("        FILE: %s\n", args->out.parsed.file);
    printf("        NAME: %s\n", args->out.parsed.name);
    printf("        EXTENSION: %s\n", args->out.parsed.extension);
    printf("     FORMATTED:\n");
    printf("        FILE: %s\n", args->out.formatted.file);
    printf("        NAME: %s\n", args->out.formatted.name);
    printf("        EXTENSION: %s\n", args->out.formatted.extension);
    printf("++++ OPTIONS ++++\n");
    printf("    PARSE: %i\n", args -> parse);
    printf("    RUN: %i\n", args -> run);
    printf("    DECODE: %i\n", args -> decode);
    printf("++++ PATHS ++++\n");
    printf("    PARSER PATH: %s\n", args -> parserPath);
    printf("    CVC5 PATH: %s\n", args -> cvc5Path);
    printf("++++ RESULT ++++\n");
    printf("    RESULT: %s\n", args -> result);
}

void printHashTable() {
    struct hashTable *entry, *tmp;
    
    printf("==== Hash Table Entries ====\n");
    
    HASH_ITER(hh, table, entry, tmp) {
        printf("Tag: %s\n", entry->tag);
        printf("  Type:   %s\n", entry->line.type);
        printf("  Rest:   %s\n", entry->line.rest);
        printf("  Rule:   %s\n", entry->line.rule);
        printf("  Prems:  %s\n", entry->line.prems);
        printf("  Args:   %s\n", entry->line.args);
        printf("  Note:   %s\n", entry->line.note);
        printf("---------------------------\n");
    }
}

void addSymbol(char *key, char *value) {
    struct dict *entry = (struct dict *) malloc(sizeof(struct dict));

    strcpy(entry->key, key);
    strcpy(entry->val, value);
    HASH_ADD_STR(symbs, key, entry);
}

void trimWhitespaces(char *str) {
    int len = strlen(str);

    // remove trailing spaces
    while (len > 0 && str[len - 1] == ' ') {
        str[len - 1] = '\0';
        len--;
    }

    // remove leading spaces
    char *start = str;
    while (*start == ' ' && *start != '\0') {
        start++;
    }

    // shift string to remove leading spaces
    if (start != str) {
        int i = 0;
        while (start[i] != '\0') {
            str[i] = start[i];
            i++;
        }
        str[i] = '\0'; // null termination
    }

    // melt multiple spaces within the string 
    char *src = str, *dest = str;
    int space = 0;

    while (*src != '\0') {
        if (*src != ' ') {
            *dest++ = *src;
            space = 0;
        } else if (!space) {
            *dest++ = ' '; // single space
            space = 1;
        }
        src++;
    }
    *dest = '\0'; // null termination
}

bool startsWith(char *str, char *comp) {
    return strncmp(str, comp, strlen(comp)) == 0;
}

char* stringCopy(char *src) {

    char *dest = malloc(strlen(src) + 1);
    if (dest == NULL) {
        return NULL;
    }

    strcpy(dest, src);
    return dest;
}

bool isEqual(char *str1, char *str2) {

    if (strlen(str1) != strlen(str2)) {
        return false;
    }
    
    return strcmp(str1, str2) == 0;
}

char *removeFileExtension(char *file) {
    
    char *copy = stringCopy(file);

    int index = getIndexOfFileExtension(copy);

    if (index > 0) {
        copy[index] = '\0';
    }

    return copy;
}

int getIndexOfFileExtension(char *file) {

    int index = -1;

    // find the last occuring dot in the string
    for (int i = 0; file[i] != '\0'; i++) {
        if (file[i] == '.') {
            index = i;
        }
    }
    return index;
}

char *getFileExtension(char *file) {

    // index of the last occuring dot in the string
    int index = getIndexOfFileExtension(file);

    // check whether extension exists
    if (index == -1) {
        return NULL;
    }

    // return extension without preceding dot
    return &file[index + 1];
}

void errNdie(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void printVarList(struct node *head) {

    if (head == NULL) {
        printf("Empty list.\n");
        return;
    }

    struct node *current = head;

    while (current != NULL) {
        struct var *var = (struct var *) current -> structure;
        if (var != NULL) {
            printf("Name: %s\n", var -> name);
            printf("Type: %s\n", var -> type);
        } else {
            errNdie("Invalid data in structure");
        }
        current = current -> next;
    }
}

void printTypeList(struct node *head) {

    if (head == NULL) {
        printf("Empty list.\n");
        return;
    }

    struct node *current = head;

    while (current != NULL) {
        struct type *type = (struct type *) current -> structure;
        if (type != NULL) {
            printf("Original: %s\n", type -> original);
            printf("Replacement: %s\n", type -> replacement);
            printf("Arity: %d\n", type -> arity);
        } else {
            errNdie("Invalid data in structure");
        }
        current = current -> next;
    }
}

struct node* addNode(void* structure, int size) {

    // allocate memory for new node
    struct node* newNode = (struct node*) malloc(sizeof(struct node));
    
    // allocate memory for the specific structure
    newNode -> structure = malloc(size);

    // copy data into the new node
    memcpy(newNode -> structure, structure, size);
    newNode -> next = NULL;
    return newNode;
}

void push(struct node** head, void* structure, int size) {
    struct node* current = *head;

    // create new list when list is empty
    if (current == NULL) {
        *head = addNode(structure, size);
        return;
    }

    // iterate to the end of the list
    while (current -> next != NULL) {
        current = current -> next;
    }

    // add new node to end of list
    current -> next = addNode(structure, size);
}
