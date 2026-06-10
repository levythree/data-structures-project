#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "simplex.h"
#include "lia.h"

int main() {
    printf("--- Complex SMT Feasibility Test ---\n");
    printf("4 Variables, 5 Inequations\n");
    
    // 4 variables (x1, x2, x3, x4), 5 equations (s1, s2, s3, s4, s5)
    // Columns = 4 (vars) + 5 (slacks) + 1 (constant) = 10 columns
    // Rows = 5 (equations) + 1 (objective) = 6 rows
    Tableau* tableau = createTableau(4, 5);
    
    // Row 0: -x1 - x2 - x3 - x4 + s1 = -10
    tableau->matrix[0][0] = -1.0; tableau->matrix[0][1] = -1.0; tableau->matrix[0][2] = -1.0; tableau->matrix[0][3] = -1.0; 
    tableau->matrix[0][4] =  1.0; tableau->matrix[0][5] =  0.0; tableau->matrix[0][6] =  0.0; tableau->matrix[0][7] =  0.0; tableau->matrix[0][8] = 0.0; 
    tableau->matrix[0][9] = -10.0;
    
    // Row 1: -2x1 - x2 + x3 - 3x4 + s2 = -5
    tableau->matrix[1][0] = -2.0; tableau->matrix[1][1] = -1.0; tableau->matrix[1][2] =  1.0; tableau->matrix[1][3] = -3.0; 
    tableau->matrix[1][4] =  0.0; tableau->matrix[1][5] =  1.0; tableau->matrix[1][6] =  0.0; tableau->matrix[1][7] =  0.0; tableau->matrix[1][8] = 0.0; 
    tableau->matrix[1][9] = -5.0;
    
    // Row 2: x1 + s3 = 4
    tableau->matrix[2][0] =  1.0; tableau->matrix[2][1] =  0.0; tableau->matrix[2][2] =  0.0; tableau->matrix[2][3] =  0.0; 
    tableau->matrix[2][4] =  0.0; tableau->matrix[2][5] =  0.0; tableau->matrix[2][6] =  1.0; tableau->matrix[2][7] =  0.0; tableau->matrix[2][8] = 0.0; 
    tableau->matrix[2][9] =  4.0;
    
    // Row 3: x2 + s4 = 3
    tableau->matrix[3][0] =  0.0; tableau->matrix[3][1] =  1.0; tableau->matrix[3][2] =  0.0; tableau->matrix[3][3] =  0.0; 
    tableau->matrix[3][4] =  0.0; tableau->matrix[3][5] =  0.0; tableau->matrix[3][6] =  0.0; tableau->matrix[3][7] =  1.0; tableau->matrix[3][8] = 0.0; 
    tableau->matrix[3][9] =  3.0;

    // Row 4: x3 + x4 + s5 = 7
    tableau->matrix[4][0] =  0.0; tableau->matrix[4][1] =  0.0; tableau->matrix[4][2] =  1.0; tableau->matrix[4][3] =  1.0; 
    tableau->matrix[4][4] =  0.0; tableau->matrix[4][5] =  0.0; tableau->matrix[4][6] =  0.0; tableau->matrix[4][7] =  0.0; tableau->matrix[4][8] = 1.0; 
    tableau->matrix[4][9] =  7.0;
    
    // Row 5: Objective Function (All Zeros since we only care about feasibility)
    tableau->matrix[5][0] = 0.0; tableau->matrix[5][1] = 0.0; tableau->matrix[5][2] = 0.0; tableau->matrix[5][3] = 0.0; 
    tableau->matrix[5][4] = 0.0; tableau->matrix[5][5] = 0.0; tableau->matrix[5][6] = 0.0; tableau->matrix[5][7] = 0.0; tableau->matrix[5][8] = 0.0; 
    tableau->matrix[5][9] = 0.0;

    int* solution = (int*) malloc(tableau->variables * sizeof(int));
    
    if (solveLIA(tableau, &solution)) {
        printf("\nResult: SAT!\n");
        printSolution(tableau, solution);
    } else {
        printf("\nResult: UNSAT!\n");
    }
    
    freeTableau(tableau);
    free(solution);

    return 0;
}
