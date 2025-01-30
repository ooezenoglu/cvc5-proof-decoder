#include "../include/decoder.h"

// TODO extend list
char *typeVariables[] = {"u", "m", "n", "p", "q"};
#define TYPEVARS_LENGTH (sizeof(typeVariables) / sizeof(char*))

char* generateTypeVar() {
    static int i = 0;

    if (i >= TYPEVARS_LENGTH) {
        errNdie("Not enough type variables");
    }

    char* type = typeVariables[i];
    i++;

    return type;
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

bool applyDeMorgansLaw(char* str) {
    bool replaced = false;

    // not(and(...))
    replaced |= replaceAll(str, "not\\(and\\(([^)]+)\\s+([^)]+)\\)\\)", "or(not (\\1) not (\\2))");

    // not(or(...))
    replaced |= replaceAll(str, "not\\(or\\(([^)]+)\\s+([^)]+)\\)\\)", "and(not (\\1) not (\\2))");

    return replaced;
}

bool simplifyImplication(char* str) {
    return replaceAll(str, "=>\\s*([A-Za-z0-9_@]+)\\s+([A-Za-z0-9_@]+)", "or(not (\\1) (\\2)");
}

bool simplifyNotExists(char* str) {
    return replaceAll(str, NOTEXISTS, FORALLNOT);
}

bool simplifyNotForall(char* str) {
    bool modified = replaceAll(str, "not forall", "exists not");

    modified |=  replaceAll(str, "\\(not \\(forall ([^ ]+) \\((.*)\\)\\)\\)", "(exists (\\1) not (\\2))");
    return modified;
}

bool simplifyDoubleNeg(char* str) {
    return replaceAll(str, NOTNOT, "");
}

bool simplifyNotFalse(char* str) {
    return replaceAll(str, NOTFALSE, TRUE);
}

bool simplifyNotTrue(char* str) {
    return replaceAll(str, NOTTRUE, FALSE);
}

void preparse() {

    FILE *proof, *preparsedProof;
    char line[2*BUFFER_SIZE];
    bool simplify;

    // store file
    strncpy(args->out.preparsed.file, args->out.raw.name, BUFFER_SIZE - 1);
    strcat(args->out.preparsed.file, "_preparsed.txt");
    args->out.preparsed.file[BUFFER_SIZE - 1] = '\0';

    // store file name
    strncpy(args->out.preparsed.name, removeFileExtension(args->out.preparsed.file), BUFFER_SIZE - 1);
    args->out.preparsed.name[BUFFER_SIZE - 1] = '\0';

    // store file extension
    strncpy(args->out.preparsed.extension, getFileExtension(args->out.preparsed.file), BUFFER_SIZE - 1);
    args->out.preparsed.extension[BUFFER_SIZE - 1] = '\0';
    
    proof = fopen(args->out.raw.file, "r+");
    preparsedProof = fopen(args->out.preparsed.file, "w+");

    if(!proof) { errNdie("Could not open proof file"); }
    if(!preparsedProof) { errNdie("Could not create preparsed proof file"); }

    while(1) {

        // read line
        if(!fgets(line, sizeof(line), proof)) {
            break; // end of file
        }

        // remove line breaks
        int len = strlen(line);
        if(len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        // the whole proof has to be preparsed again
        // if at least one simplification took place
        do {
            simplify = false;
            simplify |= simplifyImplication(line);
            simplify |= simplifyNotTrue(line);
            simplify |= simplifyNotFalse(line);
            simplify |= simplifyDoubleNeg(line);
            simplify |= simplifyNotForall(line);
            simplify |= simplifyNotExists(line);
            simplify |= applyDeMorgansLaw(line);
        } while (simplify);

        fprintf(preparsedProof, "%s\n", line);
    }
    
    fclose(proof);
    fclose(preparsedProof);
}

void refactor() {

    FILE *preparsedProof, *refactoredProof;
    char line[2*BUFFER_SIZE];

    // store file
    strncpy(args->out.refactored.file, args->out.raw.name, BUFFER_SIZE - 1);
    strcat(args->out.refactored.file, "_refactored.txt");
    args->out.refactored.file[BUFFER_SIZE - 1] = '\0';

    // store file name
    strncpy(args->out.refactored.name, removeFileExtension(args->out.refactored.file), BUFFER_SIZE - 1);
    args->out.refactored.name[BUFFER_SIZE - 1] = '\0';

    // store file extension
    strncpy(args->out.refactored.extension, getFileExtension(args->out.refactored.file), BUFFER_SIZE - 1);
    args->out.refactored.extension[BUFFER_SIZE - 1] = '\0';
    
    preparsedProof = fopen(args->out.preparsed.file, "r+");
    refactoredProof = fopen(args->out.refactored.file, "w+");

    if(!preparsedProof) { errNdie("Could not open proof file"); }
    if(!refactoredProof) { errNdie("Could not create refactored proof file"); }

     while(1) {

        // read line
        if(!fgets(line, sizeof(line), preparsedProof)) {
            break; // end of file
        }

        // remove line breaks
        int len = strlen(line);
        if(len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        // remove all comments
        if(startsWith(line, ";")) { continue; }

        // remove tptp residue
        replaceAll(line, "tptp.", "");
        
        // @list -> []
        // replaceAll(line, "@list\\s*([A-Za-z0-9_@]+)", "[\\1]");
        replaceAll(line, "@list", "");

        // remove special characters and extra spaces
        replaceAll(line, "\\$+", "");
        replaceAll(line, "\\( ", "(");
        replaceAll(line, " \\)", ")");
        replaceAll(line, "  ", " ");
    
        // extract and refactor type
        if (startsWith(line, "(declare-type")) {

            // TODO: Bool -> b; Int -> i

            char original[BUFFER_SIZE];
            char replacement[BUFFER_SIZE];

            // extract original value
            sscanf(line, "(%*s %s %*[^)]", original);

            // generate a new type variable
            strncpy(replacement, generateTypeVar(), BUFFER_SIZE - 1);
            replacement[BUFFER_SIZE - 1] = '\0';

            // add to the types list
            struct type newType;
            strncpy(newType.original, original, BUFFER_SIZE - 1);
            newType.original[BUFFER_SIZE - 1] = '\0';

            strncpy(newType.replacement, replacement, BUFFER_SIZE - 1);
            newType.replacement[BUFFER_SIZE - 1] = '\0';

            newType.arity = 0; // TODO extract arity

            push(&typeList, &newType, sizeof(struct type));
        }

        // itertively add more intuitive type names
        struct node *current = typeList;
        while (current != NULL) {
            struct type *type_data = (struct type *) current -> structure;
            if (type_data -> original[0] != '\0') {
                replaceAll(line, type_data -> original, type_data -> replacement);
            }
            current = current -> next;
        }

        // extract variable name + type
        // if (contains(line, "eo::var")) {
            
        //     char name[BUFFER_SIZE];
        //     char type[BUFFER_SIZE];

        //     // extract name and type
        //     sscanf(line, "%*[^'\"]\"%255[^\"]\" %255[^)]", name, type);

        //     // add to the list
        //     struct var new_var;
        //     strncpy(new_var.name, name, BUFFER_SIZE - 1);
        //     new_var.name[BUFFER_SIZE - 1] = '\0';

        //     strncpy(new_var.type, type, BUFFER_SIZE - 1);
        //     new_var.type[BUFFER_SIZE - 1] = '\0';

        //     push(&varList, &new_var, sizeof(struct var));
            
        //     // replaceAll(line,  "\\(eo::var [^)]*\\)", new_var.name);
        // }

        fprintf(refactoredProof, "%s\n", line);
    }

    fclose(preparsedProof);
    fclose(refactoredProof);
}

void parse() {

    FILE *refactoredProof, *parsedProof;
    char line[2*BUFFER_SIZE];
    char buf[8*BUFFER_SIZE];
    bool simplify;

    // store file
    strncpy(args->out.parsed.file, args->out.raw.name, BUFFER_SIZE - 1);
    strcat(args->out.parsed.file, "_parsed.txt");
    args->out.parsed.file[BUFFER_SIZE - 1] = '\0';

    // store file name
    strncpy(args->out.parsed.name, removeFileExtension(args->out.parsed.file), BUFFER_SIZE - 1);
    args->out.parsed.name[BUFFER_SIZE - 1] = '\0';

    // store file extension
    strncpy(args->out.parsed.extension, getFileExtension(args->out.parsed.file), BUFFER_SIZE - 1);
    args->out.parsed.extension[BUFFER_SIZE - 1] = '\0';
    
    refactoredProof = fopen(args->out.refactored.file, "r+");
    parsedProof = fopen(args->out.parsed.file, "w+");

    if(!refactoredProof) { errNdie("Could not open refactored proof file"); }
    if(!parsedProof) { errNdie("Could not create parsed proof file"); }

    while(1) {

        // read line
        if(!fgets(line, sizeof(line), refactoredProof)) {
            break; // end of file
        }

        // remove line breaks
        int len = strlen(line);
        if(len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        char type[BUFFER_SIZE] = {0};
        char tag[BUFFER_SIZE] = {0};
        char rest[BUFFER_SIZE] = {0};
        char rule[BUFFER_SIZE] = {0};
        char prems[BUFFER_SIZE] = {0};
        char args[BUFFER_SIZE] = {0};
        char note[BUFFER_SIZE] = {0};

        // extract tag
        if(contains(line, "@")) {
            sscanf(line, "(%s%*[^@]%s %[^\n]", type, tag, rest);
        } else {
            sscanf(line, "(%s %[^\n]", type, rest);
        }

        // delete last closing bracket
        rest[strlen(rest) - 1] = '\0';

        // extract more details depending on the type
        if (isEqual(type, "declare-type")) {
            sscanf(rest, "%s %[^\n]", args, note);
        
        } else if (isEqual(type, "declare-const") ) {
            sscanf(rest, "%s %[^\n]", args, note);
        
        } else if (isEqual(type, "define")) {
            if(contains(rest, "eo::var")) {
                sscanf(rest, "%[^)]%*c (eo::var \"%[^\"]\" %[^)]", prems, args, note);
            } else {
                sscanf(rest, " %[^)]%*c %[^\n]", prems, args);
            }
            strcat(prems, ")");

        } else if (isEqual(type, "assume")) {
            strcpy(args, rest);

        } else if (isEqual(type, "step")) {
            if (startsWith(rest, ":rule")) {
                sscanf(rest, ":rule %[^:]:premises %[^:]:args %[^\n]", rule, prems, args);
            } else {
                sscanf(rest, "%*[^:]:rule %[^:]:premises %[^:]:args %[^\n]", rule, prems, args);
            }

        } else { // type unknown; copy what's there
            fprintf(parsedProof, "%s\n", line);
            continue;
        }

        char *ptrArgs = args;

        // check whether tags from older steps should be replaced in args
        while ((ptrArgs = strchr(ptrArgs, '@')) != NULL) {

            char t[BUFFER_SIZE];
            struct hashTable *match = (struct hashTable *) malloc(sizeof(struct hashTable));
            int i = 0;

            // extract the tag from the rest
            while (*ptrArgs && *ptrArgs != ' ' && *ptrArgs != ')' && *ptrArgs != ']' && i < sizeof(t) - 1) {
                t[i++] = *ptrArgs++;
            }
            t[i] = '\0';

            HASH_FIND_STR(table, t, match);

            if (match) { replaceAll(args, t, match->line.args); }
            ptrArgs++;
        }
        
        char *ptrPrems = prems;

        // check whether tags from older steps should be replaced in prems
        while ((ptrPrems = strchr(ptrPrems, '@')) != NULL) {

            char t[BUFFER_SIZE];
            struct hashTable *match = (struct hashTable *) malloc(sizeof(struct hashTable));
            int i = 0;

            // extract the tag from the rest
            while (*ptrPrems && *ptrPrems != ' ' && *ptrPrems != ')' && *ptrPrems != ']' && i < sizeof(t) - 1) {
                t[i++] = *ptrPrems++;
            }
            t[i] = '\0';

            HASH_FIND_STR(table, t, match);

            if (match) { replaceAll(prems, t, match->line.args); }
            ptrPrems++;
        }

        do {
            simplify = false;
            simplify |= simplifyImplication(args);
            simplify |= simplifyNotTrue(args);
            simplify |= simplifyNotFalse(args);
            simplify |= simplifyDoubleNeg(args);
            simplify |= simplifyNotForall(args);
            simplify |= simplifyNotExists(args);
            simplify |= applyDeMorgansLaw(args);
        } while (simplify);

        // add the new entry to the hash table
        struct hashTable *entry;
        
        entry = (struct hashTable *) malloc(sizeof(struct hashTable));
        strcpy(entry->tag, tag);
        strcpy(entry->line.type, type);
        strcpy(entry->line.rest, rest);
        strcpy(entry->line.rule, rule);
        strcpy(entry->line.prems, prems);
        strcpy(entry->line.args, args);
        strcpy(entry->line.note, note);
        HASH_ADD_STR(table, tag, entry);

        snprintf(
            buf, 
            sizeof(buf),
            "%s %s %s #(%s %s)\n",
            type, args, note, 
            rule, prems
        );

        // remove unnecessary whitespace in brackets
        replaceAll(buf, "\\(\\s*\\)", "");
        replaceAll(buf, "\\(\\s+", "(");
        replaceAll(buf, "\\s+\\)", ")");
        replaceAll(buf, "\\(\\)", "");

        trimWhitespaces(buf);

        printf("%s\n", buf);
        fprintf(parsedProof, "%s", buf);
    }


    fclose(refactoredProof);
    fclose(parsedProof);
}

void formatProof() {

    char line[8*BUFFER_SIZE];
    FILE *parsedProof, *formattedProof;

    // store file
    strncpy(args->out.formatted.file, args->out.raw.name, BUFFER_SIZE - 1);
    strcat(args->out.formatted.file, "_formatted.txt");
    args->out.formatted.file[BUFFER_SIZE - 1] = '\0';

    // store file name
    strncpy(args->out.formatted.name, removeFileExtension(args->out.formatted.file), BUFFER_SIZE - 1);
    args->out.formatted.name[BUFFER_SIZE - 1] = '\0';

    // store file extension
    strncpy(args->out.formatted.extension, getFileExtension(args->out.formatted.file), BUFFER_SIZE - 1);
    args->out.formatted.extension[BUFFER_SIZE - 1] = '\0';
    
    parsedProof = fopen(args->out.parsed.file, "r+");
    formattedProof = fopen(args->out.formatted.file, "w+");

    if(!parsedProof) { errNdie("Could not open parsed proof file"); }
    if(!formattedProof) { errNdie("Could not create formatted proof file"); }

    // fixed column width for lhs
    int COLUMN_WIDTH = 90;

    while(1) {

        // read line
        if(!fgets(line, sizeof(line), parsedProof)) {
            break; // end of file
        }

        // remove line breaks
        int len = strlen(line);
        if(len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        // find first occurence of #
        char *hashPos = strchr(line, '#');
        
        if (hashPos) {
            // separate left and right side
            *hashPos = '\0';
            fprintf(formattedProof, "%-*s %s\n", COLUMN_WIDTH, line, hashPos + 1);
        } else {
            fprintf(formattedProof, "%s\n", line);
        }
    }

    fclose(parsedProof);
    fclose(formattedProof);

    // TODO
    // Infix notation
}

void decode() {

    preparse();
    refactor();
    parse();
    formatProof();

    // // debug
    // printf("++++ Type List ++++\n");
    // printTypeList(typeList);
    // printf("+++++++++++++++++++\n");
    // printf("++++ Variable List ++++\n");
    // printVarList(varList);
    // printf("+++++++++++++++++++++++\n");
        
    printHashTable();
}
