#include <stdbool.h>

#include "./nodeType.h"

#ifndef smtH
#define smtH
#define MAX_ATOMS 100
#define MAX_LEN 32
#define MAX_VARS 10

typedef struct ASTNode {
    NodeType type;
    char token[MAX_LEN];
    struct ASTNode* left;
    struct ASTNode* right;
} ASTNode;

typedef struct {
    char name[MAX_LEN];
    int column;
} Variable;

typedef struct {
    Variable vars[MAX_VARS];
    int totalVariables;
} SymbolTable;

#endif
