#include "../include/decoder.h"
#include "../include/parse_util.h"

void refactor() {

    FILE *proof, *refactoredProof;
    char line[2*BUFFER_SIZE];
    
    proof = fopen(args->proof_raw, "r+");
    refactoredProof = fopen(args->proof_ref, "w+");

    if(!proof) { errNdie("Could not open proof file"); }
    if(!refactoredProof) { errNdie("Could not create refactored proof file"); }

     while(1) {

        // read line
        if(!fgets(line, sizeof(line), proof)) {
            break; // end of file
        }

        // remove line breaks
        int len = strlen(line);
        if(len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        // remove all comments
        if(startsWith(line, ";")) { continue; }

        // remove tptp residue
        replaceAll(line, "tptp.", "");
        
        // @list -> []
        // replaceAll(line, "@list\\s*([A-Za-z0-9_@]+)", "[\\1]");
        replaceAll(line, "@list", "");

        // remove special characters and extra spaces
        replaceAll(line, "\\$+", "");
        replaceAll(line, "\\( ", "(");
        replaceAll(line, " \\)", ")");
        replaceAll(line, "  ", " ");
    
        // extract and refactor type
        if (startsWith(line, "(declare-type")) {

            // TODO: Bool -> b; Int -> i

            char original[BUFFER_SIZE];
            char replacement[BUFFER_SIZE];

            // extract original value
            sscanf(line, "(%*s %s %*[^)]", original);

            // generate a new type variable
            strncpy(replacement, generateTypeVar(), BUFFER_SIZE - 1);
            replacement[BUFFER_SIZE - 1] = '\0';

            // add to the types list
            struct type newType;
            strncpy(newType.original, original, BUFFER_SIZE - 1);
            newType.original[BUFFER_SIZE - 1] = '\0';

            strncpy(newType.replacement, replacement, BUFFER_SIZE - 1);
            newType.replacement[BUFFER_SIZE - 1] = '\0';

            newType.arity = 0; // TODO extract arity

            push(&typeList, &newType, sizeof(struct type));
        }

        // itertively add more intuitive type names
        struct node *current = typeList;
        while (current != NULL) {
            struct type *type_data = (struct type *) current -> structure;
            if (type_data -> original[0] != '\0') {
                replaceAll(line, type_data -> original, type_data -> replacement);
            }
            current = current -> next;
        }

        fprintf(refactoredProof, "%s\n", line);
    }

    fclose(proof);
    fclose(refactoredProof);
}

void parse() {

    FILE *refactoredProof, *parsedProof, *simplifiedProof;
    char line[2*BUFFER_SIZE];
    char buf[8*BUFFER_SIZE];
    char sbuf[8*BUFFER_SIZE];

    refactoredProof = fopen(args->proof_ref, "r+");
    parsedProof = fopen(args->proof_par, "w+");
    simplifiedProof = fopen(args->proof_sim, "w+");

    if(!refactoredProof) { errNdie("Could not open refactored proof file"); }
    if(!parsedProof) { errNdie("Could not open parsed proof file"); }
    if(!simplifiedProof) { errNdie("Could not create simplified proof file"); }

    while(1) {

        // read line
        if(!fgets(line, sizeof(line), refactoredProof)) {
            break; // end of file
        }

        // remove line breaks
        int len = strlen(line);
        if(len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        char type[BUFFER_SIZE] = {0};
        char tag[BUFFER_SIZE] = {0};
        char rest[BUFFER_SIZE] = {0};
        char rule[BUFFER_SIZE] = {0};
        char prems[BUFFER_SIZE] = {0};
        char sprems[BUFFER_SIZE] = {0};
        char args[BUFFER_SIZE] = {0};
        char sargs[BUFFER_SIZE] = {0};
        char note[BUFFER_SIZE] = {0};

        // extract tag
        if(contains(line, "@")) {
            sscanf(line, "(%s%*[^@]%s %[^\n]", type, tag, rest);
        } else {
            sscanf(line, "(%s %[^\n]", type, rest);
        }

        // delete last closing bracket
        rest[strlen(rest) - 1] = '\0';

        // extract more details depending on the type
        if (isEqual(type, "declare-type")) {
            sscanf(rest, "%s %[^\n]", args, note);
        
        } else if (isEqual(type, "declare-const") ) {
            sscanf(rest, "%[^ (] %[^\n]", args, note);
        
        } else if (isEqual(type, "define")) {
            if(contains(rest, "eo::var")) {
                sscanf(rest, "%[^)]%*c (eo::var \"%[^\"]\" %[^)]", prems, args, note);
            } else {
                sscanf(rest, " %[^)]%*c %[^\n]", prems, args);
            }
            strcat(prems, ")");

        } else if (isEqual(type, "assume")) {
            strcpy(args, rest);

        } else if (isEqual(type, "step")) {
            if (startsWith(rest, ":rule")) {
                sscanf(rest, ":rule %[^:]:premises %[^:]:args %[^\n]", rule, prems, args);
            } else {
                sscanf(rest, "%*[^:]:rule %[^:]:premises %[^:]:args %[^\n]", rule, prems, args);
            }

        } else { // type unknown; copy what's there
            fprintf(parsedProof, "%s\n", line);
            fprintf(simplifiedProof, "%s\n", line);
            continue;
        }

        adjustBrackets(args);

        char *ptrArgs = args;

        // check whether tags from older steps should be replaced in args
        while ((ptrArgs = strchr(ptrArgs, '@'))) {

            char t[BUFFER_SIZE];
            struct hashTable *match = (struct hashTable *) malloc(sizeof(struct hashTable));
            int i = 0;

            // extract the tag from the rest
            while (*ptrArgs && *ptrArgs != ' ' && *ptrArgs != ')' && *ptrArgs != ']' && i < sizeof(t) - 1) {
                t[i++] = *ptrArgs++;
            }
            t[i] = '\0';

            HASH_FIND_STR(table, t, match);

            if (match) { replaceAll(args, t, match->line.args.orig); }
            ptrArgs++;
        }

        removeDuplicateBrackets(args);

        strcpy(sargs, args);

        if (!startsWith(type, "declare") && strlen(sargs) > 0) {
            char simplifiedExpr[8*BUFFER_SIZE];
            
            result_ast = NULL;  // reset AST

            // scan the string to the lexer
            yy_scan_string(sargs);

            // call the parser and store resulting string 
            // simplified string is stored in global result_ast
            yyparse();
        
            // result is NULL if there is no AST
            if (result_ast != NULL) {
                memset(simplifiedExpr, 0, sizeof(simplifiedExpr));
                ast_to_string(result_ast, simplifiedExpr, sizeof(simplifiedExpr));
    
                if (strcmp(simplifiedExpr, "null") != 0) {
                    strcpy(sargs, simplifiedExpr);
                    printf("SIMPLIFIED: %s\n", sargs);
                }
            }
        }

        char *ptrPrems = prems;
        strcpy(sprems, prems);

        // check whether tags from older steps should be replaced in prems
        while ((ptrPrems = strchr(ptrPrems, '@')) != NULL) {

            char t[BUFFER_SIZE];
            struct hashTable *match = (struct hashTable *) malloc(sizeof(struct hashTable));
            int i = 0;

            // extract the tag from the rest
            while (*ptrPrems && *ptrPrems != ' ' && *ptrPrems != ')' && *ptrPrems != ']' && i < sizeof(t) - 1) {
                t[i++] = *ptrPrems++;
            }
            t[i] = '\0';

            HASH_FIND_STR(table, t, match);

            if (match) { 
                replaceAll(prems, t, match->line.args.orig); 
                replaceAll(sprems, t, match->line.args.simplified); 
            }
            ptrPrems++;
        }
        
        cleanString(type);
        cleanString(tag);
        cleanString(rest);
        cleanString(rule);
        cleanString(prems);
        cleanString(sprems);
        cleanString(args);
        cleanString(sargs);
        cleanString(note);

        // add the new entry to the hash table
        struct hashTable *entry;
        
        entry = (struct hashTable *) malloc(sizeof(struct hashTable));
        strcpy(entry->tag, tag);
        strcpy(entry->line.type, type);
        strcpy(entry->line.rest, rest);
        strcpy(entry->line.rule, rule);
        strcpy(entry->line.prems.simplified, sprems);
        strcpy(entry->line.prems.orig, prems);
        strcpy(entry->line.args.orig, args);
        strcpy(entry->line.args.simplified, sargs);
        strcpy(entry->line.note, note);
        HASH_ADD_STR(table, tag, entry);

        snprintf(buf, sizeof(buf), "%s %s %s (%s %s)\n", type, args, note, rule, prems);
        cleanString(buf);
        fprintf(parsedProof, "%s", buf);

        snprintf(sbuf, sizeof(sbuf), "%s %s %s (%s %s)\n", type, sargs, note, rule, sprems);
        cleanString(sbuf);
        fprintf(simplifiedProof, "%s", sbuf);
    }

    fclose(refactoredProof);
    fclose(parsedProof);
    fclose(simplifiedProof);
}

void formatProof() {

    int maxLength = 0;
    struct hashTable *entry, *tmp;
    FILE *formattedProof = fopen(args->proof_for, "w+");

    // first run: find max length of main part
    HASH_ITER(hh, table, entry, tmp) {

        char mainStr[4 * BUFFER_SIZE];

        snprintf(mainStr, sizeof(mainStr), "%s %s %s",
                 entry->line.type, entry->line.args.simplified, entry->line.note);

        replaceAll(mainStr, "\\(\\s*\\)", "");
        replaceAll(mainStr, "\\(\\s+", "(");
        replaceAll(mainStr, "\\s+\\)", ")");
        replaceAll(mainStr, "\\(\\)", "");
        trimWhitespaces(mainStr);

        int len = strlen(mainStr);
        if (len > maxLength) {
            maxLength = len;
        }
    }
    
    // second run: print formatted lines
    HASH_ITER(hh, table, entry, tmp) {
        char mainStr[4 * BUFFER_SIZE];
        char parenStr[4 * BUFFER_SIZE];
        char finalStr[8 * BUFFER_SIZE];
        snprintf(mainStr, sizeof(mainStr), "%s %s %s",
                 entry->line.type, entry->line.args.simplified, entry->line.note);
        replaceAll(mainStr, "\\(\\s*\\)", "");
        replaceAll(mainStr, "\\(\\s+", "(");
        replaceAll(mainStr, "\\s+\\)", ")");
        replaceAll(mainStr, "\\(\\)", "");
        trimWhitespaces(mainStr);

        snprintf(parenStr, sizeof(parenStr), "(%s %s)",
                 entry->line.rule, entry->line.prems.simplified);

        cleanString(parenStr);

        int padding = (maxLength + 2) - strlen(mainStr);
        if (padding < 0) {
            padding = 0;
        }
        snprintf(finalStr, sizeof(finalStr), "%s%*s%s", mainStr, padding, "", parenStr);
        printf("%s\n", finalStr);
        fprintf(formattedProof, "%s\n", finalStr);
    }

    fclose(formattedProof);
}

void decode() {

    refactor();
    parse();
    formatProof();

    // debug
    printHashTable();
}
