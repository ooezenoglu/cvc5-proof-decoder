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
    replaced |= replaceAll(str, "not\\(and\\(([^)]+)\\s+([^)]+)\\)\\)", "or(not \\1 not \\2)");

    // not(or(...))
    replaced |= replaceAll(str, "not\\(or\\(([^)]+)\\s+([^)]+)\\)\\)", "and(not \\1 not \\2)");

    return replaced;
}

bool simplifyImplication(char* str) {
    return replaceAll(str, "=>\\s*([A-Za-z0-9_@]+)\\s+([A-Za-z0-9_@]+)", "or(not \\1 \\2)");
}

bool simplifyNotExists(char* str) {
    return replaceAll(str, NOTEXISTS, FORALLNOT);
}

bool simplifyNotForall(char* str) {
    return replaceAll(str, NOTFORALL, EXISTSNOT);
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
    char preparsedProofFileName[2*BUFFER_SIZE];
    char line[2*BUFFER_SIZE];
    bool simplify;

    snprintf(
        preparsedProofFileName, 
        sizeof(preparsedProofFileName),
        "%s_preparsed.txt",
        args -> proofFileName
    );

    strncpy(args -> preparsedProofFile, preparsedProofFileName, BUFFER_SIZE - 1);
    args -> preparsedProofFile[BUFFER_SIZE - 1] = '\0';
    
    proof = fopen(args -> proofFile, "r+");
    preparsedProof = fopen(args -> preparsedProofFile, "w+");

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
    char refactoredProofFileName[2*BUFFER_SIZE];
    char line[2*BUFFER_SIZE];

    snprintf(
        refactoredProofFileName, 
        sizeof(refactoredProofFileName),
        "%s_refactored.txt",
        args -> proofFileName
    );

    strncpy(args -> refactoredProofFile, refactoredProofFileName, BUFFER_SIZE - 1);
    args -> refactoredProofFile[BUFFER_SIZE - 1] = '\0';
    
    preparsedProof = fopen(args -> preparsedProofFile, "r+");
    refactoredProof = fopen(args -> refactoredProofFile, "w+");

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
        replaceAll(line, "@list\\s*([A-Za-z0-9_@]+)", "[\\1]");

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
        if (contains(line, "eo::var")) {
            
            char name[BUFFER_SIZE];
            char type[BUFFER_SIZE];

            // extract name and type
            sscanf(line, "%*[^'\"]\"%255[^\"]\" %255[^)]", name, type);

            // add to the list
            struct var new_var;
            strncpy(new_var.name, name, BUFFER_SIZE - 1);
            new_var.name[BUFFER_SIZE - 1] = '\0';

            strncpy(new_var.type, type, BUFFER_SIZE - 1);
            new_var.type[BUFFER_SIZE - 1] = '\0';

            push(&varList, &new_var, sizeof(struct var));
            
            replaceAll(line,  "\\(eo::var [^)]*\\)", new_var.name);
        }

        fprintf(refactoredProof, "%s\n", line);
    }

    fclose(preparsedProof);
    fclose(refactoredProof);
}

void decode() {

    preparse();
    refactor();

    // debug
    printf("++++ Type List ++++\n");
    printTypeList(typeList);
    printf("+++++++++++++++++++\n");
    printf("++++ Variable List ++++\n");
    printVarList(varList);
    printf("+++++++++++++++++++++++\n");
}
