#include "../include/decoder.h"

void replaceAll(char* str, char* pattern, char* replacement) {

    regex_t regex;
    // matches exactly two groups, where matches[0] is the whole match
    regmatch_t matches[3];
    char buffer[2 * BUFFER_SIZE];
    char* cursor = str;

    // compile regex
    if (regcomp(&regex, pattern, REG_EXTENDED | REG_ICASE) != 0) {
        errNdie("Regex compilation failed");
    }

    buffer[0] = '\0';

    while (regexec(&regex, cursor, 3, matches, 0) == 0) {

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
}

void applyDeMorgansLaw(char* str) {
    // ~(and(A B)) <=> or(~A B)
    replaceAll(str, "not\\(and\\(\\s*([A-Za-z0-9_@]+)\\s+([A-Za-z0-9_@]+)\\)\\)", "or(not \\1 not \\2)");

    // ~(or(A B)) <=> and(~A B)
    replaceAll(str, "not\\(or\\(\\s*([A-Za-z0-9_@]+)\\s+([A-Za-z0-9_@]+)\\)\\)", "and(not \\1 not \\2)");

    // ~(and(~A ~B)) <=> or(A B)
    replaceAll(str, "not\\(and\\(\\s*not\\s*([A-Za-z0-9_@]+)\\s+not\\s*([A-Za-z0-9_@]+)\\)\\)", "or(\\1 \\2)");

    // ~(or(~A ~B)) <=> and(A B)
    replaceAll(str, "not\\(or\\(\\s*not\\s*([A-Za-z0-9_@]+)\\s+not\\s*([A-Za-z0-9_@]+)\\)\\)", "and(\\1 \\2)");
}

void simplifyImplication(char* str) {
    replaceAll(str, "=>\\s*([A-Za-z0-9_@]+)\\s+([A-Za-z0-9_@]+)", "or(not \\1 \\2)");
}

void simplifyNotExists(char* str) {
    replaceAll(str, NOTEXISTS, FORALLNOT);
}

void simplifyNotForall(char* str) {
    replaceAll(str, NOTFORALL, EXISTSNOT);
}

void simplifyDoubleNeg(char* str) {
    replaceAll(str, NOTNOT, "");
}

void simplifyNotFalse(char* str) {
    replaceAll(str, NOTFALSE, TRUE);
}

void simplifyNotTrue(char* str) {
    replaceAll(str, NOTTRUE, FALSE);
}

void preparse() {

    FILE *proof, *preparsedProof;
    char preparsedProofFileName[2*BUFFER_SIZE];
    char line[2*BUFFER_SIZE];

    snprintf(
        preparsedProofFileName, 
        sizeof(preparsedProofFileName),
        "%s_preparsed.txt",
        removeFileExtension(args -> proofFile)
        );
    
    proof = fopen(args -> proofFile, "r+");
    preparsedProof = fopen(preparsedProofFileName, "w+");

    if(!proof) { errNdie("Could not open proof file"); }
    if(!preparsedProof) { errNdie("Could not create preparsed proof file"); }

        while(1) {

            // read line
            if(!fgets(line, sizeof(line), proof)) {
                break; // end of file
            }

            // remove line breaks
            size_t len = strlen(line);
            if(len > 0 && line[len - 1] == '\n') {
                line[len - 1] = '\0';
            }

            simplifyImplication(line);
            simplifyNotTrue(line);
            simplifyNotFalse(line);
            simplifyDoubleNeg(line);
            simplifyNotForall(line);
            simplifyNotExists(line);
            applyDeMorgansLaw(line);

            fprintf(preparsedProof, "PARSED: %s\n", line);
    }
    
    fclose(proof);
    fclose(preparsedProof);
}

void decode() {

    preparse();
}
