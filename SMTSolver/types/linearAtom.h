#ifndef linearAtomH
#define linearAtomH

typedef struct {
    int numberOfVariables;
    double* coefficients;
    double rhs;
} LinearAtom;

LinearAtom* createLinearAtom(int numberOfVariables, double* parsedCoefficients, double parsedRHS);

void freeLinearAtom(LinearAtom* linearAtom);

#endif
