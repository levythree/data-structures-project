#ifndef simplexH
#define simplexH

#include <stdbool.h>

typedef struct {
    int variables;
    int equations;
    int rows;
    int columns;
    double** matrix;
} Tableau;

Tableau* createTableau(int rows, int columns);

void freeTableau(Tableau* tableau);

void pivot(Tableau* tableau, int pivotRow, int pivotColumn);

bool simplex(Tableau* tableau);

#endif
