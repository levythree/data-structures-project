#ifndef cnfH
#define cnfH

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

Formula* createFormula(int numberOfClauses, int numberOfVariables);

Formula* readCnf(const char* cnf);

int evaluateFormula(Formula* formula, int* interpretation);

bool sat(Formula* formula, int* interpretation, int currentVariable);

#endif
