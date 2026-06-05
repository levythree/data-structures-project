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
    struct satNode *left;
    struct satNode *right;
} SATNode;

void initClause(Clause* clause, int* temporaryBuffer, int numberOfLiterals);

Formula* createFormula(int numberOfClauses, int numberOfVariables);

SATNode* createSATNode(int variableId, int valueAssigned);

Formula* readCnf(const char* cnf);

int evaluateFormula(Formula* formula, int* interpretation);

bool sat(Formula* formula, int* interpretation, int currentVariable, SATNode** satNode);

void printSATTree(SATNode* satNode);

#endif
