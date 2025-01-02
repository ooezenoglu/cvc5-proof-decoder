#include "../include/helpers.h"
#include "../include/encoder.h"
#include "../include/decoder.h"

struct node *typeList;
struct node *varList;
struct type *types;
struct var *vars;
struct args *args;
struct hashTable *table;

void runCvc5();

int main(int argc, char *argv[]) {

    // struct to store input arguments
    args = malloc(sizeof(struct args));
    memset(args, 0, sizeof(struct args));

    table = NULL;

    extractCommandLineArgs(argc, argv);

    // parse .p into .smt2 when --p flag is set
    if(args -> parse == 1 && strlen(args->in.p.file) > 0) {
        runCvc5Parser();
    }

    // intuitive representation of problem when --s flag is set
    if(args -> simplify == 1 && strlen(args->in.smt2.file) > 0) {
        // TODO: run encoder here
    }
    
    // run cvc5
    if(args -> run == 1 && strlen(args->in.smt2.file) > 0) {
        runCvc5();
    }

    if(args -> decode == 1) {
        decode();
    }

    // debug
    printArgsStruct();

    // cleanup
    free(args);
    return 0;
}

void runCvc5() {

    char command[4*BUFFER_SIZE];

    setExecPermissions(args->cvc5Path);

    // store file
    strncpy(args->out.raw.file, args->in.smt2.name, BUFFER_SIZE - 1);
    strcat(args->out.raw.file, "_proof.txt");
    args->out.raw.file[BUFFER_SIZE - 1] = '\0';

    // store file name
    strncpy(args->out.raw.name, removeFileExtension(args->out.raw.file), BUFFER_SIZE - 1);
    args->out.raw.name[BUFFER_SIZE - 1] = '\0';

    // store file extension
    strncpy(args->out.raw.extension, getFileExtension(args->out.raw.file), BUFFER_SIZE - 1);
    args->out.raw.extension[BUFFER_SIZE - 1] = '\0';
    
    snprintf(
        command, 
        sizeof(command),
        "%s %s --dump-proofs --force-logic='HO_ALL' > %s",
        args->cvc5Path,
        args->in.smt2.file,
        args->out.raw.file
    );
    
    // execute command
    if (system(command) == -1) {
        errNdie("Could not execute the cvc5 parser command");
    }
}