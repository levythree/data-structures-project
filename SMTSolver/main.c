#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "simplex.h"

int main() {
    // 2x1 +  x2 <= 10
    // -x1  + 2x2 <= 6

    Tableau* tableau1 = createTableau(2, 2);

    // linha 0: 2x1 + x2 + s1 = 10
    tableau1->matrix[0][0] = 2.0;
    tableau1->matrix[0][1] = 1.0;
    tableau1->matrix[0][2] = 1.0; 
    tableau1->matrix[0][3] = 0.0;
    tableau1->matrix[0][4] = 10.0;
    
    // linha 1: -x1 + 2x2 + s2 = 6
    tableau1->matrix[1][0] = -1.0;
    tableau1->matrix[1][1] = 2.0;
    tableau1->matrix[1][2] = 0.0; 
    tableau1->matrix[1][3] = 1.0;
    tableau1->matrix[1][4] = 6.0;
    
    // linha 2: objetivo [-1, -2]
    tableau1->matrix[2][0] = -1.0;
    tableau1->matrix[2][1] = -2.0;
    tableau1->matrix[2][2] = 0.0; 
    tableau1->matrix[2][3] = 0.0;
    tableau1->matrix[2][4] = 0.0;

    if (simplex(tableau1)) {
        printf("SAT!\n");

        printSolution(tableau1);
    }
    else printf("UNSAT!\n");

    freeTableau(tableau1);

    // 0x1 + 0x2 <= -5

    Tableau* tableau2 = createTableau(2, 1);
    
    // linha 1: 0x1 + 0x2 + s1 = -5
    tableau2->matrix[0][0] = 0.0;
    tableau2->matrix[0][1] = 0.0;
    tableau2->matrix[0][2] = 1.0; 
    tableau2->matrix[0][3] = -5.0;
    
    // Row 1: objetivo [0, 0]
    tableau2->matrix[1][0] = 0.0;
    tableau2->matrix[1][1] = 0.0;
    tableau2->matrix[1][2] = 0.0; 
    tableau2->matrix[1][3] = 0.0;

    if (simplex(tableau2)) {
        printf("SAT!\n");

        printSolution(tableau2);
    } else printf("UNSAT!\n");

    freeTableau(tableau2);

    return 0;
}
