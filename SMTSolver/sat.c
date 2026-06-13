#include "./types/sat.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void initClause(Clause* clause, int* temporaryBuffer, int numberOfLiterals) {
    clause->literals = (int*)malloc(numberOfLiterals * sizeof(int));
    clause->numberOfLiterals = numberOfLiterals;

    int i;

    for (i = 0; i < numberOfLiterals; i++) {
        clause->literals[i] = temporaryBuffer[i];
    }
}

Formula* createFormula(int numberOfVariables, int numberOfClauses) {
    Formula* formula = (Formula*)malloc(sizeof(Formula));

    formula->clauses = (Clause*)malloc(numberOfClauses * sizeof(Clause));
    formula->numberOfVariables = numberOfVariables;
    formula->numberOfClauses = numberOfClauses;

    return formula;
}

int evaluateFormula(Formula* formula, int* interpretation) {
    bool allClausesTrue = true;

    for (int i = 0; i < formula->numberOfClauses; i++) {
        bool clauseIsTrue = false, clauseHasUnassigned = false;

        for (int j = 0; j < formula->clauses[i].numberOfLiterals; j++) {
            int literal = formula->clauses[i].literals[j];
            int variableId = abs(literal);
            int variableValue = interpretation[variableId];

            int literalTruthValue = 0;
            
            if (variableValue != 0) literalTruthValue = (literal > 0) ? variableValue : -variableValue;

            if (literalTruthValue == 1) {
                clauseIsTrue = true;

                break;
            } else if (literalTruthValue == 0) clauseHasUnassigned = true;
        }

        if (!clauseIsTrue) {
            if (!clauseHasUnassigned) return -1;
            else allClausesTrue = false;
        }
    }

    if (allClausesTrue) return 1;

    return 0;
}

void freeFormula(Formula* formula) {
    if (formula == NULL) return;

    for (int i = 0; i < formula->numberOfClauses; i++) {
        if (formula->clauses[i].literals) free(formula->clauses[i].literals);
    }

    free(formula->clauses);
    free(formula);
}
