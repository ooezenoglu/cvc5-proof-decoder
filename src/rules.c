#include "../include/rules.h"

bool applyDeMorgansLaw(char* str) {
    bool replaced = false;

    // not(and(...))
    replaced |= replaceAll(str, "not\\(and\\(([^)]+)\\s+([^)]+)\\)\\)", "or(not (\\1) not (\\2))");

    // not(or(...))
    replaced |= replaceAll(str, "not\\(or\\(([^)]+)\\s+([^)]+)\\)\\)", "and(not (\\1) not (\\2))");

    return replaced;
}

bool simplifyImplication(char* str) {
    return replaceAll(str, "=>\\s*([A-Za-z0-9_@]+)\\s+([A-Za-z0-9_@]+)", "or(not (\\1) (\\2)");
}

bool simplifyNotExists(char* str) {
    return replaceAll(str, NOTEXISTS, FORALLNOT);
}

bool simplifyNotForall(char* str) {
    bool modified = replaceAll(str, NOTFORALL, EXISTSNOT);

    modified |=  replaceAll(str, "\\(not \\(forall ([^ ]+) \\((.*)\\)\\)\\)", "(exists (\\1) not (\\2))");
    return modified;
}

bool simplifyDoubleNeg(char* str) {
    return replaceAll(str, NOTNOT, "");
}

bool simplifyNotFalse(char* str) {
    return replaceAll(str, NOTFALSE, TRUE);
}

bool simplifyNotTrue(char* str) {
    return replaceAll(str, NOTTRUE, FALSE);
}
