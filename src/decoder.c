#include "../include/decoder.h"

char* generateTypeVar() {
    static int i = 0;

    if (i >= TYPEVARS_LENGTH) {
        errNdie("Not enough type variables");
    }

    char* type = typeVars[i];
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

        // remove unnecessary details
        replaceAll(line, "eo::var", "");
        
        // @list -> []
        replaceAll(line, "@list\\s*([A-Za-z0-9_@]+)", "[\\1]");
        
        // add intuitive variable names
        // TODO generic function
        replaceAll(line, "\\$\\$[A-Za-z0-9_@]+", "u");

        fprintf(refactoredProof, "%s\n", line);
    }

    fclose(preparsedProof);
    fclose(refactoredProof);
}

void decode() {

    preparse();
    refactor();
}
