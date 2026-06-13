#ifndef linearAtomH
#define linearAtomH
#include "./nodeType.h"

typedef struct {
    int numberOfVariables;
    double* coefficients;
    double rhs;
    NodeType type;
} LinearAtom;

LinearAtom createLinearAtom(int numberOfVariables, double* parsedCoefficients,
                            double parsedRHS);

void freeLinearAtom(LinearAtom* linearAtom);

#endif
