#include "../include/helpers.h"
#include "../include/encoder.h"
#include "../include/decoder.h"

struct node *typeList;
struct node *varList;
struct type *types;
struct var *vars;
struct args *args;

int main(int argc, char *argv[]) {

    // struct to store input arguments
    args = malloc(sizeof(struct args));
    memset(args, 0, sizeof(struct args));

    extractCommandLineArgs(argc, argv);

    // parse .p into .smt2 when --p flag is set
    if(args -> parse == 1 && strlen(args->in.p.file) > 0) {
        runCvc5Parser();
    }

    // intuitive representation of problem when --s flag is set
    if(args -> simplify == 1 && strlen(args->in.smt2.file) > 0) {
        // TODO: run encoder here
    }

    if(args -> decode == 1) {
        // store proof file
        strncpy(args->out.raw.file, "../FOL/SEU_FOL_unsat_proof.txt", BUFFER_SIZE - 1);
        args->out.raw.file[BUFFER_SIZE - 1] = '\0';

        // store proof file name
        strncpy(args->out.raw.name, removeFileExtension(args->out.raw.file), BUFFER_SIZE - 1);
        args->out.raw.name[BUFFER_SIZE - 1] = '\0';

        // store proof file extension
        strncpy(args->out.raw.extension, getFileExtension(args->out.raw.file), BUFFER_SIZE - 1);
        args->out.raw.extension[BUFFER_SIZE - 1] = '\0';

        decode();
    }

    // debug
    printArgsStruct();

    // cleanup
    free(args);
    return 0;
}
