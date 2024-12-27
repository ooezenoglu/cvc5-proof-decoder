#include "../include/helpers.h"

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

    printf("FILE: %s\n", args -> file);
    printf("FILE NAME: %s\n", args -> fileName);
    printf("FILE EXTENSION: %s\n", args -> fileExtension);
    printf("PARSE: %i\n", args -> parse);
    printf("SIMPLIFY: %i\n", args -> simplify);
    printf("DECODE: %i\n", args -> decode);
    printf("PARSER PATH: %s\n", args -> parserPath);
    printf("CVC5 PATH: %s\n", args -> cvc5Path);
    printf("PROOF FILE: %s\n", args -> proofFile);
    printf("PROOF FILE NAME: %s\n", args -> proofFileName);
    printf("PROOF FILE EXTENSION: %s\n", args -> proofFileExtension);
    printf("PREPARSED PROOF FILE: %s\n", args -> preparsedProofFile);
    printf("REFACTORED PROOF FILE: %s\n", args -> refactoredProofFile);
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
