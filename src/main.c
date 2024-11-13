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

    // cleanup
    free(args);
    return 0;
}
