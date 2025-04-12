% Axiom 1
fof(axiom_1, axiom,
    ~ ( ! [A] : (subset(A, A) => subset(A, A))) ).

% Axiom 2
fof(axiom_2, axiom,
    ! [A] : subset(A, A) ).