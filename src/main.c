#include "../include/helpers.h"
#include "../include/encoder.h"
#include "../include/decoder.h"
#include "../include/parse_util.h"

// #include "../include/parser.tab.h"

struct node *typeList;
struct node *varList;
struct type *types;
struct var *vars;
struct args *args;
struct hashTable *table;
struct dict *symbs;

// #include "parser.tab.h"

// declare Flex functions
extern int yyparse(void);
extern void yy_scan_string(const char*);

void runCvc5();

int main(int argc, char *argv[]) {

    // struct to store input arguments
    args = malloc(sizeof(struct args));
    memset(args, 0, sizeof(struct args));

    table = NULL;
    symbs = NULL;

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

        FILE *proof = fopen(args->out.raw.file, "r+");
    
        if (fgets(args->result, sizeof(args->result), proof)) {
            // remove line break if it exists
            args->result[strcspn(args->result, "\n")] = '\0';
        }

        // set up output files
        generateOutputFile(args->out.preparsed.file, args->out.raw.name, "_preparsed.txt");
        generateOutputFile(args->out.refactored.file, args->out.raw.name, "_refactored.txt");
        generateOutputFile(args->out.parsed.file, args->out.raw.name, "_parsed.txt");
        generateOutputFile(args->out.formatted.file, args->out.raw.name, "_formatted.txt");

        // TODO error handling
        
        printArgsStruct();
        decode();

        char simplifiedExpr[8*BUFFER_SIZE];

        const char *tests[] = {
            "(=> (A) (B))",
            "(=> (subset (A) (A)) (subset (A) (A)))",
            "(not (not (A)))",
            "(not (not (not (not (A)))))",
            "(not (=> (A) (B)))",
            "(not (not (not (=> (S) (k)))))",
            "(not (forall (A) (=> (A) (B))))",
            "(not (and (A) (B)))",            
            "(not (or (A) (B)))",            
            "(not (forall (x) (A)))",            
            "(not (exists (x) (A)))",
            "(not (and (=> (A) (B)) (not (not (C)))))",
            "(= (A) (B))",            
            "((A))",
            "(forall (A) (=> (A) (B)))",
            "(not (forall (A) (=> (and (Z) (X)) (B))))"
        };
        
        int numTests = sizeof(tests) / sizeof(tests[0]);
        
        for (int i = 0; i < numTests; i++) {
            printf("Orig string: %s\n", tests[i]);
            yy_scan_string(tests[i]);
            yyparse();
            memset(simplifiedExpr, 0, sizeof(simplifiedExpr));
            ast_to_string(result_ast, simplifiedExpr, sizeof(simplifiedExpr));
            printf("New string: %s\n\n", simplifiedExpr);
        }
        
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

    // store file, file name, file extension
    generateOutputFile(args->out.raw.file, args->in.smt2.name, "_proof.txt");
    generateOutputFile(args->out.raw.name, removeFileExtension(args->out.raw.file), "");
    generateOutputFile(args->out.raw.extension, getFileExtension(args->out.raw.file), "");
    
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