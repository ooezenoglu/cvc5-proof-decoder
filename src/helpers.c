#include "../include/helpers.h"

char *typeVariables[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};
#define TYPEVARS_LENGTH (sizeof(typeVariables) / sizeof(char*))

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

void adjustBrackets(char *str) {
    int len = strlen(str);
    char temp[BUFFER_SIZE];

    // wrap brackets around the string if not done already 
    if (len == 0 || str[0] != '(' || str[len - 1] != ')') {
        snprintf(temp, BUFFER_SIZE, "(%s)", str);
        strncpy(str, temp, BUFFER_SIZE - 1);
        str[BUFFER_SIZE - 1] = '\0';
        len = strlen(str);
    }

    // remove duplicate brackets ((...))
    if (len >= 4 && str[0] == '(' && str[1] == '(' &&
        str[len - 2] == ')' && str[len - 1] == ')') {
        // copy content between first and last bracket
        strncpy(temp, str + 1, len - 2);
        temp[len - 2] = '\0';
        strncpy(str, temp, BUFFER_SIZE - 1);
        str[BUFFER_SIZE - 1] = '\0';
    }
}

void removeDuplicateBrackets(char *str) {

    bool changed = true;

    while (changed) {

        changed = false;
        int len = strlen(str);

        // iterate over string
        for (int i = 0; i < len - 1; i++) {
            if (str[i] == '(' && str[i + 1] == '(') {
                int depth = 1;
                int j = i + 2;
                // find matching closing bracket
                while (j < len && depth > 0) {
                    if (str[j] == '(')
                        depth++;
                    else if (str[j] == ')')
                        depth--;
                    j++;
                }

                // if pattern (( ... )) found, remove duplicate brackets
                if (depth == 0 && j < len && str[j] == ')') {
                    char temp[BUFFER_SIZE];
                    int pos = 0;
                    for (int k = 0; k <= i; k++)
                        temp[pos++] = str[k];
                    for (int k = i + 2; k < j; k++)
                        temp[pos++] = str[k];
                    for (int k = j + 1; k < len; k++)
                        temp[pos++] = str[k];
                    temp[pos] = '\0';
                    strncpy(str, temp, BUFFER_SIZE);
                    str[BUFFER_SIZE - 1] = '\0';
                    changed = true;
                    break;
                }
            }
        }
    }
}

void cleanString(char *str) {

    // remove unnecessary whitespace in brackets
    replaceAll(str, "\\(\\s*\\)", "");
    replaceAll(str, "\\(\\s+", "(");
    replaceAll(str, "\\s+\\)", ")");
    replaceAll(str, "\\(\\)", "");

    trimWhitespaces(str);
}

char* generateTypeVar() {
    
    static int i = 0;

    if (i >= TYPEVARS_LENGTH) {
        errNdie("Not enough type variables");
    }

    char* type = typeVariables[i];
    i++;

    return type;
}

void printArgsStruct() {

    printf("++++ INPUT ++++\n");
    printf("     P FILE: %s\n", args->p_file);
    printf("     SMT2 FILE: %s\n", args->smt2_file);
    printf("++++ OUTPUT ++++\n");
    printf("     RAW PROOF: %s\n", args->proof_raw);
    printf("     REFACTORED PROOF: %s\n", args->proof_ref);
    printf("     PARSED PROOF: %s\n", args->proof_par);
    printf("     SIMPLIFIED PROOF: %s\n", args->proof_sim);
    printf("     FORMATTED PROOF: %s\n", args->proof_for);
    printf("++++ OPTIONS ++++\n");
    printf("    PARSE: %i\n", args -> parse);
    printf("    RUN: %i\n", args -> run);
    printf("    DECODE: %i\n", args -> decode);
    printf("++++ PATHS ++++\n");
    printf("    CVC5 PARSER: %s\n", args -> parserPath);
    printf("    CVC5 SOLVER: %s\n", args -> cvc5Path);
    printf("++++ RESULT ++++\n");
    printf("    RESULT: %s\n\n", args -> result);
}

void printHashTable() {
    struct hashTable *entry, *tmp;
    
    printf("==== Hash Table Entries ====\n");
    
    HASH_ITER(hh, table, entry, tmp) {
        printf("Tag: %s\n", entry->tag);
        printf("  Type:   %s\n", entry->line.type);
        printf("  Body:   %s\n", entry->line.body);
        printf("  Rule:   %s\n", entry->line.rule);
        printf("  Prems:  %s\n", entry->line.prems.orig);
        printf("  Simplified Prems:  %s\n", entry->line.prems.simplified);
        printf("  Args:   %s\n", entry->line.args.orig);
        printf("  Simplified Args:  %s\n", entry->line.args.simplified);
        printf("  Note:   %s\n", entry->line.note);
        printf("---------------------------\n");
    }
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

bool replaceAll(char* str, char* pattern, char* replacement) {

    regex_t regex;
    // matches exactly two groups, where matches[0] is the whole match
    regmatch_t matches[3];
    char buffer[2 * BUFFER_SIZE];
    char* cursor = str;
    bool matched = false;

    // compile regex
    if (regcomp(&regex, pattern, REG_EXTENDED | REG_ICASE) != 0) {
        errNdie("Regex compilation failed");
    }

    buffer[0] = '\0';

    while (regexec(&regex, cursor, 3, matches, 0) == 0) {

        matched = true;

        // copy string before the match
        strncat(buffer, cursor, matches[0].rm_so);

        // process the replacement pattern
        char* rCursor = replacement;

        while (*rCursor != '\0') {

            // for binary operations, replace \1 or \2 with respective group content
            if (*rCursor == '\\' && (*(rCursor + 1) == '1' || *(rCursor + 1) == '2')) {
                // extract the group number
                int groupIndex = *(rCursor + 1) - '0';
                // calculate length of the group content
                int length = matches[groupIndex].rm_eo - matches[groupIndex].rm_so;
                // add group content
                strncat(buffer, cursor + matches[groupIndex].rm_so, length);
                rCursor += 2; // skip escape char + digit

            } else {
                // copy every other character
                strncat(buffer, rCursor, 1);
                rCursor++;
            }
        }

        // move cursor to behind the match
        cursor += matches[0].rm_eo;
    }

    // copy rest of the string
    strcat(buffer, cursor);

    // modify original string
    strncpy(str, buffer, BUFFER_SIZE - 1);

    // safety measure
    str[BUFFER_SIZE - 1] = '\0'; 

    // cleanup
    regfree(&regex);

    return matched;
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

void openFile(char *output, char *input, char *suffix) {

    // extract the name of input without extension
    char *base = removeFileExtension(input);

    // append suffix
    snprintf(output, BUFFER_SIZE, "%s%s", base, suffix);

    // create the file
    FILE *f = fopen(output, "w");
    if (f == NULL) {
        errNdie("Could not create the file");
    }
    fclose(f);

    // cleanup
    free(base);
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

void errNdie(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void extractCommandLineArgs(int argc, char *argv[]) {

    int opt;
    int fflag = 0;
    int pflag = 0;
    int rflag = 0;
    int cflag = 0;
    char *extension;

    while((opt = getopt(argc, argv, ":f:pdc:r:")) != -1) {

        switch(opt) {

            // file option
            case 'f':

                // check whether file name exists
                if(optarg != NULL) {

                    if(!(extension = getFileExtension(optarg))) {
                        errNdie("File has no valid file extension");
                    }
                    
                    // check whether file extension is supported
                    if(isEqual(extension, P) || isEqual(extension, SMT2)) {
                        fflag = 1;

                        if(isEqual(extension, P)) {
                            // store file
                            strncpy(args->p_file, optarg, BUFFER_SIZE - 1);
                            args->p_file[BUFFER_SIZE - 1] = '\0';
                        }
                        
                        if(isEqual(extension, SMT2)) {
                            // store file
                            strncpy(args->smt2_file, optarg, BUFFER_SIZE - 1);
                            args->smt2_file[BUFFER_SIZE - 1] = '\0';
                        }
                    }

                    if(fflag == 0) {
                        errNdie("File type is not supported");
                    }
                }
                break;

            // parse option
            case 'p':
                args -> parse = 1;
                pflag = 1;
                break;

            // run option
            case 'r':
                args -> run = 1;
                rflag = 1;

                 if(optarg != NULL) {
                    // store cvc5 path
                    strncpy(args -> cvc5Path, optarg, BUFFER_SIZE - 1);
                    args -> cvc5Path[BUFFER_SIZE - 1] = '\0';
                }
                break;

            // parser path
            case 'c':
                if(optarg != NULL) {
                    cflag = 1;

                    // store parser path
                    strncpy(args -> parserPath, optarg, BUFFER_SIZE - 1);
                    args -> parserPath[BUFFER_SIZE - 1] = '\0';

                    break;
                }
            
            // decode option
            case 'd':
                args -> decode = 1;
                break;

            default:
                if (opt == ':') {  // missing argument
                    printf("Option --%c requires an argument\n", optopt);
                    errNdie("");
                }
                break;
        }
    }

    if(fflag == 0) {
        errNdie("Missing file name");
    }

    if(pflag == 1 && cflag == 0) {
        errNdie("Missing parser path");
    }
    
    if(rflag == 1 && strlen(args -> cvc5Path) <= 0) {
        errNdie("Missing cvc5 path");
    }
}