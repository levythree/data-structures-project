#ifndef astH
#define astH

#define MAX_LEN 32
#define MAX_VARS 100

typedef enum {
    NODE_VAR,
    NODE_NUM,
    NODE_OP_ADD,
    NODE_OP_SUB,
    NODE_OP_EQ,
    NODE_OP_LE,
    NODE_OP_GE,
    NODE_OP_MUL,
    NODE_OP_OR
} NodeType;

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

ASTNode* createNode(NodeType type, const char* token);

void freeASTNode(ASTNode* node);

void initSymbolTable(SymbolTable* table);

int getOrCreateVariableColumn(SymbolTable* table, char* name);

ASTNode* cloneAST(ASTNode* node);

void freeAST(ASTNode* node);

#endif
