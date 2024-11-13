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
    printf("EXTENSION: %s\n", args -> extension);
    printf("PARSE: %i\n", args -> parse);
    printf("SIMPLIFY: %i\n", args -> simplify);
    printf("PARSER PATH: %s\n", args -> parserPath);
    printf("CVC5 PATH: %s\n", args -> cvc5Path);
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