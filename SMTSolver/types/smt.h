#ifndef smtH
#define smtH

#include <stdbool.h>

#include "ast.h"
#include "atom.h"
#include "sat.h"
#include "simplex.h"

void linearizeExpression(ASTNode* node, Tableau* tableau, int row, double sign, SymbolTable* table);

LinearAtom extractLinearAtom(ASTNode* node, SymbolTable* table, bool invert);

void parseToLinearAtoms(ASTNode* node, LinearAtom* linearAtoms, int* counter, SymbolTable* table);

bool atomsMatch(LinearAtom a, LinearAtom b, int numVars);

void mapAstToCnfBuffer(ASTNode* node, LinearAtom* linearAtoms, int totalAtoms, int* clauseBuffer, int* literalCount, SymbolTable* table);

void desugarEqualities(ASTNode** array, int* assertionsCount);

void printSolution(SymbolTable* table, int* solution);

bool checkTheory(int* interpretation, int totalVariables, SymbolTable* table, LinearAtom* linearAtoms);

bool satSMT(Formula* formula, int* interpretation, int currentVariable, SymbolTable* table, LinearAtom* linearAtoms);

bool smt(ASTNode** array, int assertionsCount, LinearAtom* linearAtoms, int linearAtomsCount, SymbolTable* table);

#endif
