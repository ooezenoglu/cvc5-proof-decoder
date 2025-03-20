#include "../include/helpers.h"
#include "../include/parse_util.h"

struct node *typeList;
struct node *varList;
struct type *types;
struct var *vars;
struct args *args;
struct hashTable *table;

// declare Flex functions
extern int yyparse(void);
extern void yy_scan_string(const char*);

void runCvc5Parser();
void runCvc5();
void testExpressions();

int main(int argc, char *argv[]) {

    // struct to store input arguments
    args = malloc(sizeof(struct args));
    memset(args, 0, sizeof(struct args));

    table = NULL;

    extractCommandLineArgs(argc, argv);

    // parse .p into .smt2 when --p flag is set
    if(args -> parse == 1 && strlen(args->p_file) > 0) {
        runCvc5Parser();
    }
    
    // run cvc5
    if(args -> run == 1 && strlen(args->smt2_file) > 0) {
        runCvc5();
    }

    if(args -> decode == 1) {

        FILE *proof = fopen(args->proof_raw, "r+");
    
        if (fgets(args->result, sizeof(args->result), proof)) {
            // remove line break if it exists
            args->result[strcspn(args->result, "\n")] = '\0';
        }

        fclose(proof);

        if(!isEqual(args->result, "unsat")) {
            errNdie("The input problem is not unsat");
        }

        // set up output files
        openFile(args->proof_ref, args-> proof_raw, "_refactored.txt");
        openFile(args->proof_par, args -> proof_raw, "_parsed.txt");
        openFile(args->proof_sim, args -> proof_raw, "_simplified.txt");
        openFile(args->proof_for, args -> proof_raw, "_formatted.txt");
        
        decode();
    }

    // debug
    // printArgsStruct();
    // testExpressions();

    // cleanup
    free(args);
    return 0;
}

void runCvc5Parser() {

    char command[4*BUFFER_SIZE];

    setExecPermissions(args -> parserPath);

    // set up the smt2 file
    openFile(args->smt2_file, args -> p_file, ".smt2");

    snprintf(
            command, 
            sizeof(command),
            "%s -o raw-benchmark --parse-only --output-lang=smt2 %s > %s",
            args->parserPath,
            args->p_file,
            args->smt2_file
            );
    
    // execute command
    if (system(command) == -1) {
        errNdie("Could not execute the cvc5 parser command");
    }
}

void runCvc5() {

    char command[4*BUFFER_SIZE];

    setExecPermissions(args->cvc5Path);

    // set up the proof file
    openFile(args->proof_raw, args->smt2_file, "_proof.txt");
    
    snprintf(
        command, 
        sizeof(command),
        "%s %s --dump-proofs --force-logic='HO_ALL' > %s",
        args->cvc5Path,
        args->smt2_file,
        args->proof_raw
    );
    
    // execute command
    if (system(command) == -1) {
        errNdie("Could not execute the cvc5 parser command");
    }
}

void testExpressions() {
    char simplifiedExpr[8 * BUFFER_SIZE];

    char *validTests[] = {
        // basic logical operations
        "(=> (A) (B))",
        "(=> (subset (A) (A)) (subset (A) (A)))",
        "(not (not (A)))",
        "(not (not (not (not (A)))))",
        "(not (=> (A) (B)))",
        "(not (not (not (=> (A) (B)))))",
        
        // quantifiers and negations
        "(not (forall (A) (=> (A) (B))))",
        "(not (forall (x) (A)))",
        "(not (exists (x) (A)))",
        "(forall (A) (=> (A) (B)))",
        "(not (forall (A) (=> (and (Z) (X)) (B))))",
        
        // logical operators
        "(not (and (A) (B)))",
        "(not (or (A) (B)))",
        "(not (and (=> (A) (B)) (not (not (C)))))",
        "(= (A) (B))",
        "((A))",
        
        // nested expressions
        "(=> (or (>= (x) 2) (not (>= (x) 1))) false)",
        "(=> (forall (x) (or (>= (x) 2) (not (>= (x) 1)))) false)",
        "(= (=> (or (>= (x) 2) (not (>= (x) 1))) false) (not (forall (x) (or (>= (x) 2) (not (>= (x) 1))))))",
        "(not (exists (?X) (>= (- 5) (?X))))",
        "(not (forall (?X) (= (- (- (?X))) (?X))))",
        
        // additional test cases
        "A",
        "123",
        "((A) (B))",
        "(f (A) (B) (C))",
        "(forall (x) (exists (y) (and (P x) (Q y))))",
        "(=> (A) (=> (B) (C)))",
        "(and (or (A) (B)) (not (C)))",
        "(not (or (and (A) (B)) (and (C) (D))))",
        "(not (not (not (and (A) (or (B) (C))))))"
    };    

    char *invalidTests[] = {
        "(=> (A) (B)",
        "(and (A) (B)",
        "(not (A)) extra",
        "not (A)",
        "(and (A))",
        "(forall (x) (A)",
        "(exists (x) A))",
        "(not (and (A) (B) (C)))",
        "(=> (A) )",
        "()"
    };    
    
    int numValidTests = sizeof(validTests) / sizeof(validTests[0]);
    int numInvalidTests = sizeof(invalidTests) / sizeof(invalidTests[0]);

    printf("+++++ Valid Expressions +++++\n\n");
    for (int i = 0; i < numValidTests; i++) {

        result_ast = NULL;

        printf("Test string: %s\n", validTests[i]);

        yy_scan_string(validTests[i]);
        yyparse();

        memset(simplifiedExpr, 0, sizeof(simplifiedExpr));
        ast_to_string(result_ast, simplifiedExpr, sizeof(simplifiedExpr));

        printf("Simplified string: %s\n\n", simplifiedExpr);
    }

    memset(simplifiedExpr, 0, sizeof(simplifiedExpr));

    printf("+++++ Invalid Expressions +++++\n\n");
    for (int i = 0; i < numInvalidTests; i++) {

        result_ast = NULL;

        printf("Test string: %s\n", invalidTests[i]);

        yy_scan_string(invalidTests[i]);
        yyparse();

        memset(simplifiedExpr, 0, sizeof(simplifiedExpr));
        ast_to_string(result_ast, simplifiedExpr, sizeof(simplifiedExpr));

        printf("Simplified string: %s\n\n", simplifiedExpr);
    }

}