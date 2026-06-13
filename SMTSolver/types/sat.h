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

typedef struct satNode {
    int variableId;
    int valueAssigned;
    struct satNode* left;
    struct satNode* right;
} SATNode;

void initClause(Clause* clause, int* temporaryBuffer, int numberOfLiterals);

Formula* createFormula(int numberOfVariables, int numberOfClauses);

SATNode* createSATNode(int variableId, int valueAssigned);

#endif
