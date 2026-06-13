#include "./types/ast.h"

#include <stdlib.h>
#include <string.h>

ASTNode* createNode(NodeType type, const char* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;

    strcpy(node->token, token);

    node->left = NULL;
    node->right = NULL;

    return node;
}

void initSymbolTable(SymbolTable* table) {
    table->totalVariables = 0;
}

int getOrCreateVariableColumn(SymbolTable* table, char* name) {
    for (int i = 0; i < table->totalVariables; i++) {
        if (strcmp(table->vars[i].name, name) == 0) {
            return table->vars[i].column;
        }
    }

    strcpy(table->vars[table->totalVariables].name, name);
    table->vars[table->totalVariables].column = table->totalVariables;
    table->totalVariables++;

    return table->totalVariables - 1;
}

ASTNode* cloneAST(ASTNode* node) {
    if (node == NULL) return NULL;
    
    ASTNode* newNode = createNode(node->type, node->token);

    newNode->left = cloneAST(node->left);
    newNode->right = cloneAST(node->right);
    
    return newNode;
}

void freeAST(ASTNode* node) {
    if (node == NULL) return;

    freeAST(node->left);
    freeAST(node->right);

    free(node);
}
