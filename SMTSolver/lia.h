#ifndef liaH
#define liaH

#include "simplex.h"

double getVariableValue(Tableau* tableau, int variableIndex);

Tableau* cloneTableau(Tableau* oldTableau);

Tableau* cloneAndExpand(Tableau* oldTableau);

bool solveLIA(Tableau* tableau, int** solutions);

void printSolution(Tableau* tableau, int* solution);

#endif
