#ifndef rules
#define rules

#include "helpers.h"

bool applyDeMorgansLaw(char* str);
bool simplifyImplication(char* str);
bool simplifyNotExists(char* str);
bool simplifyNotForall(char* str);
bool simplifyDoubleNeg(char* str);
bool simplifyNotFalse(char* str);
bool simplifyNotTrue(char* str);

#endif