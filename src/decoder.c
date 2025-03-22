#include "../include/decoder.h"
#include "../include/parse_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 256

int lengthMainString = 0;

void extractComponents(char *line, char *tag, char *type, char *body) {

    char buffer[2 * BUFFER_SIZE];
    
    // temporal buffer
    strncpy(buffer, line, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    // remove line breaks
    int len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
    
    // remove opening and closing brackets
    len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == ')') {
        buffer[len - 1] = '\0';
    }
    if (buffer[0] == '(') {
        memmove(buffer, buffer + 1, strlen(buffer));
    }
    
    type[0] = '\0';
    tag[0] = '\0';
    body[0] = '\0';
    
    // tokenize line
    char *token = strtok(buffer, " \t");
    if (token == NULL) { return; }
    
    // first word is always the tpye
    strncpy(type, token, BUFFER_SIZE - 1);
    type[BUFFER_SIZE - 1] = '\0';
    
    // get the next token
    token = strtok(NULL, " \t");
    
    if (token != NULL && token[0] == '@') {
        // tag, if word begins with @
        strncpy(tag, token, BUFFER_SIZE - 1);
        tag[BUFFER_SIZE - 1] = '\0';
        
        // the rest of the words is the body
        char bodyBuffer[2 * BUFFER_SIZE] = "";
        token = strtok(NULL, " \t");
        if (token != NULL) {
            strncpy(bodyBuffer, token, sizeof(bodyBuffer) - 1);
            bodyBuffer[sizeof(bodyBuffer) - 1] = '\0';
        }
        while ((token = strtok(NULL, " \t")) != NULL) {
            strcat(bodyBuffer, " ");
            strcat(bodyBuffer, token);
        }
        strncpy(body, bodyBuffer, 2 * BUFFER_SIZE - 1);
        body[2 * BUFFER_SIZE - 1] = '\0';
    } else {
        // no tag present, everything is body
        char bodyBuffer[2 * BUFFER_SIZE] = "";
        if (token != NULL) {
            strncpy(bodyBuffer, token, sizeof(bodyBuffer) - 1);
            bodyBuffer[sizeof(bodyBuffer) - 1] = '\0';
        }
        while ((token = strtok(NULL, " \t")) != NULL) {
            strcat(bodyBuffer, " ");
            strcat(bodyBuffer, token);
        }
        strncpy(body, bodyBuffer, 2 * BUFFER_SIZE - 1);
        body[2 * BUFFER_SIZE - 1] = '\0';
    }
}

int extractSubcomponents(char *type, char *body, char *args, char *prems, char *note, char *rule) {

    // extract more details depending on the type
    if (isEqual(type, "declare-type")) {
        sscanf(body, "%s %[^\n]", args, note);
    
    } else if (isEqual(type, "declare-const") ) {
        sscanf(body, "%[^ (] %[^\n]", args, note);
    
    } else if (isEqual(type, "define")) {
        if (contains(body, "eo::var")) {
            sscanf(body, "%[^)]%*c (eo::var \"%[^\"]\" %[^)]", prems, args, note);
        } else {
            sscanf(body, " %[^)]%*c %[^\n]", prems, args);
        }
        strcat(prems, ")");
    
    } else if (isEqual(type, "assume")) {
        strcpy(args, body);
    
    } else if (isEqual(type, "step")) {
        if (startsWith(body, ":rule")) {
            sscanf(body, ":rule %[^:]:premises %[^:]:args %[^\n]", rule, prems, args);
        } else {
            sscanf(body, "%*[^:]:rule %[^:]:premises %[^:]:args %[^\n]", rule, prems, args);
        }
    
    } else {
        return 1; // unknown Typ
    }
    return 0;
}

void resolveCrossReferences(char *body) {

    char *ptr;

    // while a cross-reference exists
    while ((ptr = strchr(body, '@')) != NULL) {

        char t[BUFFER_SIZE];
        int i = 0;

        // extract the tag from the body
        while (ptr[i] && ptr[i] != ' ' && ptr[i] != ')' && ptr[i] != ']' && i < BUFFER_SIZE - 1) {
            t[i] = ptr[i];
            i++;
        }
        t[i] = '\0';

        // Suche in der Hashmap (table) nach diesem Tag
        struct hashTable *match = NULL;
        HASH_FIND_STR(table, t, match);

        if (match) {
            // replace the Tag in the body
            replaceAll(body, t, match->line.args.orig);
        } else {
            // skip
            ptr++;
        }
    }
}

void simplifyExpression(char *expr) {
    if (expr == NULL || strlen(expr) == 0) {
        return;
    }
    
    char simplifiedExpr[8 * BUFFER_SIZE];
    
    result_ast = NULL;  // reset AST

    // scan the string to the lexer
    yy_scan_string(expr);

    // call the parser and store resulting string 
    // simplified string is stored in global result_ast
    yyparse();
    
    // result is NULL if there is no AST
    if (result_ast != NULL) {
        memset(simplifiedExpr, 0, sizeof(simplifiedExpr));
        ast_to_string(result_ast, simplifiedExpr, sizeof(simplifiedExpr));
    
        if (strcmp(simplifiedExpr, "null") != 0) {
            strcpy(expr, simplifiedExpr);
            // printf("SIMPLIFIED: %s\n", expr);
        }
    }
}

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
        replaceAll(line, "\\?+", "");
        replaceAll(line, "\\( ", "(");
        replaceAll(line, " \\)", ")");
        replaceAll(line, "  ", " ");
    
        // extract and refactor type
        if (startsWith(line, "(declare-type")) {

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

            // newType.arity = 0; // TODO extract arity

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

    FILE *refactoredProof, *parsedProof;
    char line[2*BUFFER_SIZE];
    char buf[8*BUFFER_SIZE];

    if(!(refactoredProof = fopen(args->proof_ref, "r+"))) { errNdie("Could not open refactored proof file"); }
    if(!(parsedProof = fopen(args->proof_par, "w+"))) { errNdie("Could not open parsed proof file"); }

    fprintf(parsedProof, "%s\n", args->result);

    while(1) {

        // read line
        if(!fgets(line, sizeof(line), refactoredProof)) {
            break; // end of file
        }

        // remove line breaks
        line[strcspn(line, "\n")] = '\0';

        char type[BUFFER_SIZE] = {0};
        char tag[BUFFER_SIZE] = {0};
        char body[BUFFER_SIZE] = {0};
        char resolved_body[BUFFER_SIZE] = {0};
        char rule[BUFFER_SIZE] = {0};
        char prems[BUFFER_SIZE] = {0};
        char args[BUFFER_SIZE] = {0};
        char note[BUFFER_SIZE] = {0};

        // extract components <TYPE><TAG><BODY>
        extractComponents(line, tag, type, body);

        // resolve references to other proof lines
        strcpy(resolved_body, body);
        resolveCrossReferences(resolved_body);

        // extract more details depending on the type
        if (extractSubcomponents(type, resolved_body, args, prems, note, rule) == 1) {
            // type unknown; copy what's there
            strcpy(note, line);
            continue;
        }
        
        cleanString(type);
        cleanString(tag);
        cleanString(rule);
        cleanString(prems);
        cleanString(args);
        cleanString(note);

        // add the new entry to the hash table
        struct hashTable *entry;
        
        entry = (struct hashTable *) malloc(sizeof(struct hashTable));
        strcpy(entry->tag, tag);
        strcpy(entry->line.type, type);
        strcpy(entry->line.body, body);
        strcpy(entry->line.resolved_body, resolved_body);
        strcpy(entry->line.rule, rule);
        strcpy(entry->line.prems.orig, prems);
        strcpy(entry->line.args.orig, args);
        strcpy(entry->line.note, note);
        HASH_ADD_STR(table, tag, entry);

        snprintf(buf, sizeof(buf), "%s %s %s (%s %s)\n", type, args, note, rule, prems);
        cleanString(buf);
        fprintf(parsedProof, "%s", buf);
    }

    fclose(refactoredProof);
    fclose(parsedProof);
}

void simplify() {

    FILE *simplifiedProof;
    struct hashTable *entry, *tmp;
    char sbuf[8*BUFFER_SIZE];

    if(!(simplifiedProof = fopen(args->proof_sim, "w+"))) { errNdie("Could not create simplified proof file"); }
    
    fprintf(simplifiedProof, "%s\n", args->result);

    HASH_ITER(hh, table, entry, tmp) {

        char sprems[BUFFER_SIZE] = {0};
        char sargs[BUFFER_SIZE] = {0};

        strcpy(sargs, entry->line.args.orig);
        strcpy(sprems, entry->line.prems.orig);

        // prepare for simplifcation
        removeDuplicateBrackets(sargs);
        removeDuplicateBrackets(sprems);

        // simplify logical expressions
        simplifyExpression(sargs);
        simplifyExpression(sprems);

        cleanString(sargs);
        cleanString(sprems);

        strcpy(entry->line.prems.simplified, sprems);
        strcpy(entry->line.args.simplified, sargs);

         // extract the length of the LHS for later formatting
         char mainStr[4 * BUFFER_SIZE];
         snprintf(mainStr, sizeof(mainStr), "%s %s %s", entry -> line.type, sargs, entry->line.note);
         int len = strlen(mainStr);
         if (len > lengthMainString) { lengthMainString = len; }

        snprintf(sbuf, sizeof(sbuf), "%s %s %s (%s %s)\n", 
                 entry->line.type, entry->line.args.simplified, 
                 entry->line.note, entry->line.rule, entry->line.prems.simplified);
        cleanString(sbuf);
        fprintf(simplifiedProof, "%s", sbuf);
    }
    fclose(simplifiedProof);
}

void formatProof() {

    struct hashTable *entry, *tmp;
    FILE *formattedProof = fopen(args->proof_for, "w+");

    fprintf(formattedProof, "%s\n", args->result);

    // print proof lines of the form <TYPE><ARGS><NOTE>     (<RULE> <PREMS>)
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

        int padding = (lengthMainString + 2) - strlen(mainStr);
        if (padding < 0) { padding = 0; }

        snprintf(finalStr, sizeof(finalStr), "%s%*s%s", mainStr, padding, "", parenStr);
        fprintf(formattedProof, "%s\n", finalStr);
    }

    fclose(formattedProof);
}

void decode() {

    refactor();
    parse();
    simplify();
    formatProof();

    // debug
    printHashTable();
}
