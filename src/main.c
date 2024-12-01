#include "../include/helpers.h"
#include "../include/encoder.h"
#include "../include/decoder.h"

struct args *args;

int main(int argc, char *argv[]) {

    // struct to store input arguments
    args = malloc(sizeof(struct args));
    memset(args, 0, sizeof(struct args));

    extractCommandLineArgs(argc, argv);

    // parse .p files when --p or --s flags are set
    if((args -> parse == 1 || args -> simplify == 1) && isEqual(args -> extension, P) == 1) {
        runCvc5Parser();
    }

    if(args -> decode == 1) {
        strncpy(args -> proofFile, "../FOL/SEU_FOL_unsat_proof.txt", BUFFER_SIZE - 1);
        args -> proofFile[BUFFER_SIZE - 1] = '\0';

        decode();
    }

    // debug
    printArgsStruct();

    // cleanup
    free(args);
    return 0;
}
