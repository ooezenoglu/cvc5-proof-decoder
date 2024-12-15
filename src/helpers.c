#include "../include/helpers.h"

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

void print_list(struct typevar * head) {
    struct typevar * current = head;

    while (current != NULL) {
        printf("Original: %s\n", current->original);
        printf("Replacement: %s\n", current->replacement);
        printf("Arity: %d\n", current->arity);

        current = current->next;
    }
}

void push(struct typevar *head, char *original, char *replacement, int arity) {

    struct typevar *current = head;

    // use the first node if it is still empty
    if (current->original[0] == '\0' && current->replacement[0] == '\0') {
        strncpy(current->original, original, BUFFER_SIZE);
        current->original[BUFFER_SIZE - 1] = '\0';

        strncpy(current->replacement, replacement, BUFFER_SIZE);
        current->replacement[BUFFER_SIZE - 1] = '\0';

        current->arity = arity;
        current->next = NULL;
        return;
    }

    while (current->next != NULL) {
        current = current->next;
    }

    // add a new type variable
    current->next = (struct typevar *) malloc(sizeof(struct typevar));

    strncpy(current->next->original, original, BUFFER_SIZE);
    current->next->original[BUFFER_SIZE - 1] = '\0';

    strncpy(current->next->replacement, replacement, BUFFER_SIZE);
    current->next->replacement[BUFFER_SIZE - 1] = '\0';

    current->next->arity = arity;
    current->next->next = NULL;
}

void pop(struct typevar ** head) {
    struct typevar * next_node = NULL;

    if (*head == NULL) {
        errNdie("List is empty.");
    }

    next_node = (*head)->next;
    free(*head);
    *head = next_node;
}
