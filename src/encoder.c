#include "../include/encoder.h"

void runCvc5Parser() {

    char command[4*BUFFER_SIZE];

    setExecPermissions(args -> parserPath);

    // store file
    strncpy(args->in.smt2.file, args->in.p.name, BUFFER_SIZE - 1);
    strcat(args->in.smt2.file, ".smt2");
    args->in.smt2.file[BUFFER_SIZE - 1] = '\0';

    // store file name
    strncpy(args->in.smt2.name, args->in.p.name, BUFFER_SIZE - 1);
    args->in.smt2.name[BUFFER_SIZE - 1] = '\0';

    // store file extension
    strncpy(args->in.smt2.extension, getFileExtension(args->in.smt2.file), BUFFER_SIZE - 1);
    args->in.smt2.extension[BUFFER_SIZE - 1] = '\0';

    snprintf(
            command, 
            sizeof(command),
            "%s -o raw-benchmark --parse-only --output-lang=smt2 %s > %s",
            args->parserPath,
            args->in.p.file,
            args->in.smt2.file
            );
    
    // execute command
    if (system(command) == -1) {
        errNdie("Could not execute the cvc5 parser command");
    }
}

void extractCommandLineArgs(int argc, char *argv[]) {

    int opt;
    int fflag = 0;
    int pflag = 0;
    int rflag = 0;
    int cflag = 0;
    char *extension;

    while((opt = getopt(argc, argv, ":f:psdc:r:")) != -1) {

        switch(opt) {

            // file option
            case 'f':

                // check whether file name exists
                if(optarg != NULL) {

                    if(!(extension = getFileExtension(optarg))) {
                        errNdie("File has no valid file extension");
                    }
                    
                    // check whether file extension is supported
                    if(isEqual(extension, P) || isEqual(extension, SMT2)) {
                        fflag = 1;

                        if(isEqual(extension, P)) {
                            // store file
                            strncpy(args->in.p.file, optarg, BUFFER_SIZE - 1);
                            args->in.p.file[BUFFER_SIZE - 1] = '\0';

                            // store file name
                            strncpy(args->in.p.name, removeFileExtension(optarg), BUFFER_SIZE - 1);
                            args->in.p.name[BUFFER_SIZE - 1] = '\0';

                            // store file extension
                            strncpy(args->in.p.extension, extension, BUFFER_SIZE - 1);
                            args->in.p.extension[BUFFER_SIZE - 1] = '\0';
                        }
                        
                        if(isEqual(extension, SMT2)) {
                            // store file
                            strncpy(args->in.smt2.file, optarg, BUFFER_SIZE - 1);
                            args->in.smt2.file[BUFFER_SIZE - 1] = '\0';

                            // store file name
                            strncpy(args->in.smt2.name, removeFileExtension(optarg), BUFFER_SIZE - 1);
                            args->in.smt2.name[BUFFER_SIZE - 1] = '\0';

                            // store file extension
                            strncpy(args->in.smt2.extension, extension, BUFFER_SIZE - 1);
                            args->in.smt2.extension[BUFFER_SIZE - 1] = '\0';
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

            // run option
            case 'r':
                args -> run = 1;
                rflag = 1;

                 if(optarg != NULL) {
                    // store cvc5 path
                    strncpy(args -> cvc5Path, optarg, BUFFER_SIZE - 1);
                    args -> cvc5Path[BUFFER_SIZE - 1] = '\0';
                }
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
            
            // decode option
            case 'd':
                args -> decode = 1;
                break;

            default:
                if (opt == ':') {  // missing argument
                    printf("Option --%c requires an argument\n", optopt);
                    errNdie("");
                }
                break;
        }
    }

    if(fflag == 0) {
        errNdie("Missing file name");
    }

    if(pflag == 1 && cflag == 0) {
        errNdie("Missing parser path");
    }
    
    if(rflag == 1 && strlen(args -> cvc5Path) <= 0) {
        errNdie("Missing cvc5 path");
    }
}