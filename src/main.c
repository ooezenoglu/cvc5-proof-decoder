#include "../include/helpers.h"
#include "../include/encoder.h"

struct args *args;

int main(int argc, char *argv[]) {

    // struct to store input arguments
    args = malloc(sizeof(struct args));
    memset(args, 0, sizeof(struct args));

    extractCommandLineArgs(argc, argv);

    // debug
    printArgsStruct();

    // parse .p files when --p or --s flags are set
    if((args -> parse == 1 || args -> simplify == 1) && isEqual(args -> extension, P) == 1) {
        runCvc5Parser();
    }

    // cleanup
    free(args);
    return 0;
}
