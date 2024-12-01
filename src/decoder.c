#include "../include/decoder.h"

char* simplifyNotExists(char* str) {

    char* res = str;

    // replace "not exists" with "forall not"
    while ((res = strstr(res, NOTEXISTS)) != NULL) {

        int remainderLen = strlen(res + strlen(NOTEXISTS));

        // shift the remainder to the right to make room for the replacement
        memmove(res + strlen(FORALLNOT), res + strlen(NOTEXISTS), remainderLen + 1);

        // copy the replacement string into place
        memcpy(res, FORALLNOT, strlen(FORALLNOT));

        // move the pointer to after the replacement
        res += strlen(FORALLNOT);
    }

    return str;
}

char* simplifyNotForall(char* str) {

    char* res = str;

    // replace "not forall" with "exists not"
    while ((res = strstr(res, NOTFORALL)) != NULL) {

        int remainderLen = strlen(res + strlen(NOTFORALL));

        // shift the remainder to the right to make room for the replacement
        memmove(res + strlen(EXISTSNOT), res + strlen(NOTFORALL), remainderLen + 1);

        // copy the replacement string into place
        memcpy(res, EXISTSNOT, strlen(EXISTSNOT));

        // move the pointer to after the replacement
        res += strlen(EXISTSNOT);
    }

    return str;
}

char* simplifyDoubleNeg(char* str) {

    char* res = str;

    while ((res = strstr(res, NOTNOT)) != NULL) {

        // itertively delete all double negations in the string and copy the rest
        memmove(res, res + strlen(NOTNOT) + 1, strlen(res + strlen(NOTNOT) + 1) + 1);
    }
    return str;
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

            simplifyDoubleNeg(line);
            simplifyNotForall(line);
            simplifyNotExists(line);

            fprintf(preparsedProof, "PARSED: %s\n", line);
    }
        
    fclose(proof);
    fclose(preparsedProof);
}

void decode() {

    preparse();
}
