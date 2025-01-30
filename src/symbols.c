#include "../include/rules.h"

void importSymbols() {
    addSymbol("forall", "∀");
    addSymbol("exists", "∃");
    addSymbol("subset", "⊆");
    addSymbol("not", "~");
    addSymbol("or", "∨");
    addSymbol("and", "∧");
    addSymbol("->", "→");
}