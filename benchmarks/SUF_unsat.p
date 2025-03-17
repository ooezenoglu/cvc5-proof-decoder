% Axiom 1: Not all sets are subsets of themselves (negated universal quantifier with implication).
fof(axiom_1, axiom,
    ~ ( ! [A] : (subset(A, A) => subset(A, A))) ).

% Axiom 2: Every set A is a subset of itself (reflexivity of subset relation).
fof(axiom_2, axiom,
    ! [A] : subset(A, A) ).
