#ifndef liaH
#define liaH

#include <stdbool.h>

#include "simplex.h"

double getVariableValue(Tableau* tableau, int variableIndex);

Tableau* cloneTableau(Tableau* oldTableau);

Tableau* cloneAndExpand(Tableau* oldTableau);

bool solveLIA(Tableau* tableau, int** solutions);

#endif
