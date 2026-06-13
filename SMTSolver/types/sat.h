#ifndef satH
#define satH

typedef struct {
    int* literals;
    int numberOfLiterals;
} Clause;

typedef struct {
    Clause* clauses;
    int numberOfVariables;
    int numberOfClauses;
} Formula;

void initClause(Clause* clause, int* temporaryBuffer, int numberOfLiterals);

Formula* createFormula(int numberOfVariables, int numberOfClauses);

int evaluateFormula(Formula* formula, int* interpretation);

void freeFormula(Formula* formula);

#endif
