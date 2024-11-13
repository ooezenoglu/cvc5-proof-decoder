#include "../include/encoder.h"

void runCvc5Parser() {

    char command[4*BUFFER_SIZE];

    setExecPermissions(args -> parserPath);
    
    snprintf(
            command, 
            sizeof(command),
            "%s -o raw-benchmark --parse-only --output-lang=smt2 %s > %s.smt2",
            args -> parserPath,
            args -> file,
            args -> fileName
            );
    
    if (system(command) == -1) {
        errNdie("Could not execute the cvc5 parser command");
    }
}

void extractCommandLineArgs(int argc, char *argv[]) {

    int opt;
    int fflag = 0;
    int pflag = 0;
    int sflag = 0;
    int cflag = 0;
    char *extension;
    char *validExtensions[NUM_EXTENSIONS] = {P, SMT2};

    while((opt = getopt(argc, argv, ":f:psc:")) != -1) {

        switch(opt) {

            // file option
            case 'f':

                // check whether file name exists
                if(optarg != NULL) {

                    if(!(extension = getFileExtension(optarg))) {
                        errNdie("File has no valid file extension");
                    }
                    
                    // check whether file extension is supported
                    for(int i = 0; i < NUM_EXTENSIONS; i++) {
                        if(isEqual(extension, validExtensions[i])) {
                            fflag = 1;

                            // store file
                            strncpy(args -> file, optarg, BUFFER_SIZE - 1);
                            args -> file[BUFFER_SIZE - 1] = '\0';

                            // store file name
                            strncpy(args -> fileName, removeFileExtension(optarg), BUFFER_SIZE - 1);
                            args -> fileName[BUFFER_SIZE - 1] = '\0';

                            // store file extension
                            strncpy(args -> extension, extension, BUFFER_SIZE - 1);
                            args -> extension[BUFFER_SIZE - 1] = '\0';
                        }
                    }

                    if(fflag == 0) {
                        errNdie("File type is not supported");
                    }
                }
                break;

            // parse option
            case 'p':
                args -> parse = 1;
                pflag = 1;
                break;

            // simplify option
            case 's':
                args -> simplify = 1;
                sflag = 1;
                break;

            // parser path
            case 'c':
                if(optarg != NULL) {
                    cflag = 1;

                    // store parser path
                    strncpy(args -> parserPath, optarg, BUFFER_SIZE - 1);
                    args -> parserPath[BUFFER_SIZE - 1] = '\0';

                    break;
                }
        }
    }

    if(fflag == 0) {
        errNdie("Missing file name");
    }

    if((pflag == 1 || sflag == 1) && cflag == 0) {
        errNdie("Missing parser path");
    }
}