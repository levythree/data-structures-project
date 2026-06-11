#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./types/lia.h"

#define MAX_LEN 32
#define MAX_VARS 10

typedef enum {
    NODE_VAR,
    NODE_NUM,
    NODE_OP_ADD,
    NODE_OP_SUB,
    NODE_OP_EQ,
    NODE_OP_LE,
    NODE_OP_GE
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

bool lexerNextToken(FILE* file, char* token) {
    int character;

    while (true) {
        character = fgetc(file);
        if (character == EOF) return false;

        if (isspace(character)) continue;

        if (character == ';') {
            while ((character = fgetc(file)) != '\n' && character != EOF);
            continue;
        }

        break;
    }

    if (character == '(' || character == ')') {
        token[0] = character;
        token[1] = '\0';
        return true;
    }

    int len = 0;
    while (character != EOF && !isspace(character) && character != '(' &&
           character != ')') {
        if (len < MAX_LEN - 1) token[len++] = character;

        character = fgetc(file);
    }
    token[len] = '\0';

    if (character != EOF) ungetc(character, file);

    return true;
}

int lexerPeekChar(FILE* file) {
    int character;

    while (true) {
        character = fgetc(file);
        if (character == EOF) return EOF;

        if (isspace(character)) continue;

        if (character == ';') {
            while ((character = fgetc(file)) != '\n' && character != EOF);
            continue;
        }

        break;
    }

    if (character != EOF) ungetc(character, file);

    return character;
}

ASTNode* createNode(NodeType type, const char* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;

    strcpy(node->token, token);

    node->left = NULL;
    node->right = NULL;

    return node;
}

NodeType getNodeType(char* token) {
    if (strcmp(token, "+") == 0) return NODE_OP_ADD;
    if (strcmp(token, "-") == 0) return NODE_OP_SUB;
    if (strcmp(token, "=") == 0) return NODE_OP_EQ;
    if (strcmp(token, "<=") == 0) return NODE_OP_LE;
    if (strcmp(token, ">=") == 0) return NODE_OP_GE;

    return NODE_VAR;
}

ASTNode* binaryTreeParser(FILE* file) {
    char token[MAX_LEN];

    int nextChar = lexerPeekChar(file);

    if (nextChar == '(') {
        lexerNextToken(file, token);

        char op[MAX_LEN];
        lexerNextToken(file, op);

        NodeType opType = getNodeType(op);
        ASTNode* current_node = createNode(opType, op);

        current_node->left = binaryTreeParser(file);

        if (lexerPeekChar(file) != ')')
            current_node->right = binaryTreeParser(file);

        while (lexerPeekChar(file) != ')') {
            ASTNode* new_parent = createNode(opType, op);
            new_parent->left = current_node;
            new_parent->right = binaryTreeParser(file);
            current_node = new_parent;
        }

        lexerNextToken(file, token);
        return current_node;
    }

    if (!lexerNextToken(file, token)) return NULL;

    if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
        return createNode(NODE_NUM, token);
    } else {
        return createNode(NODE_VAR, token);
    }
}

void printInOrder(ASTNode* tree, int depth) {
    if (!tree) return;

    printInOrder(tree->right, depth + 1);

    for (int i = 0; i < depth; i++) {
        printf("        ");
    }

    if (strcmp(tree->token, "-") == 0 && tree->right == NULL)
        printf("[ - unário ]\n");
    else
        printf("[ %s ]\n", tree->token);

    printInOrder(tree->left, depth + 1);
}

void parseSMTFile(FILE* file, ASTNode** array, int* counter) {
    char token[32];

    while (lexerNextToken(file, token)) {
        if (strcmp(token, "(") == 0) {
            char command[32];
            lexerNextToken(file, command);

            if (strcmp(command, "declare-const") == 0) {
                char variable[32];
                char type[32];

                lexerNextToken(file, variable);
                lexerNextToken(file, type);

                lexerNextToken(file, token);
                printf("[Gerenciador] Registrada variável inteira: %s\n",
                       variable);

            } else if (strcmp(command, "assert") == 0) {
                printf("[Gerenciador] Entrando na região do assert...\n");

                ASTNode* tree = binaryTreeParser(file);
                array[(*counter)++] = tree;

                printf(
                    "[Gerenciador] Árvore do assert construída com sucesso!\n");

                lexerNextToken(file, token);

            } else if (strcmp(command, "check-sat") == 0) {
                printf("[Gerenciador] Comando check-sat detectado!\n");

                lexerNextToken(file, token);

            } else {
                int openedParentheses = 1;
                while (openedParentheses > 0) {
                    lexerNextToken(file, token);
                    if (strcmp(token, "(") == 0) openedParentheses++;
                    if (strcmp(token, ")") == 0) openedParentheses--;
                }
            }
        }
    }
}

void initSymbolTable(SymbolTable* table) { table->totalVariables = 0; }

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

void linearizeExpression(ASTNode* node, Tableau* tableau, int row, double sign,
                         SymbolTable* table) {
    if (!node) return;

    if (tableau == NULL) {
        if (node->type == NODE_VAR)
            getOrCreateVariableColumn(table, node->token);

        linearizeExpression(node->left, NULL, row, sign, table);
        linearizeExpression(node->right, NULL, row, sign, table);
        return;
    }

    switch (node->type) {
        case NODE_NUM:
            tableau->matrix[row][tableau->columns - 1] -=
                (atof(node->token) * sign);
            break;

        case NODE_VAR:
            int col = getOrCreateVariableColumn(table, node->token);
            tableau->matrix[row][col] += (1.0 * sign);
            break;

        case NODE_OP_ADD:
            linearizeExpression(node->left, tableau, row, sign, table);
            linearizeExpression(node->right, tableau, row, sign, table);
            break;

        case NODE_OP_SUB:
            if (node->right == NULL) {
                linearizeExpression(node->left, tableau, row, -sign, table);
            } else {
                linearizeExpression(node->left, tableau, row, sign, table);
                linearizeExpression(node->right, tableau, row, -sign, table);
            }
            break;

        case NODE_OP_EQ:
        case NODE_OP_LE:
        case NODE_OP_GE:
            linearizeExpression(node->left, tableau, row, sign, table);
            linearizeExpression(node->right, tableau, row, sign, table);
            break;

        default:
            break;
    }
}

void convertAssertionToRow(ASTNode* tree, Tableau* tableau, int row,
                           SymbolTable* table) {
    if (!tree) return;

    switch (tree->type) {
        case NODE_OP_LE:
            linearizeExpression(tree->left, tableau, row, 1.0, table);
            linearizeExpression(tree->right, tableau, row, -1.0, table);
            break;

        case NODE_OP_GE:
            linearizeExpression(tree->left, tableau, row, -1.0, table);
            linearizeExpression(tree->right, tableau, row, 1.0, table);
            break;

        case NODE_OP_EQ:
            linearizeExpression(tree->left, tableau, row, 1.0, table);
            linearizeExpression(tree->right, tableau, row, -1.0, table);
            break;

        default:
            break;
    }
}

bool smt(ASTNode** array, int size) {
    if (size == 0) return 1;
    SymbolTable table;
    initSymbolTable(&table);

    for (int i = 0; i < size; i++) {
        printInOrder(array[i], 0);
        linearizeExpression(array[i], NULL, 0, 1.0, &table);
    }

    int equationsQuantity = size;
    Tableau* tableau = createTableau(table.totalVariables, equationsQuantity);

    for (int i = 0; i < size; i++)
        convertAssertionToRow(array[i], tableau, i, &table);

    int* solution = (int*)malloc(table.totalVariables * sizeof(int));

    printf("\n[SMT] Resolvendo LIA localmente com %d variáveis...\n",
           table.totalVariables);

    bool result = solveLIA(tableau, &solution);

    if (result) {
        printSolution(tableau, solution);
    }

    free(solution);
    freeTableau(tableau);

    return result;
}

int main(int argc, char* argv[]) {
    FILE* file = fopen(argv[1], "r");

    ASTNode** array = malloc(10 * sizeof(ASTNode*));
    int counter = 0;

    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        free(array);
        return 1;
    }

    parseSMTFile(file, array, &counter);

    if (smt(array, counter))
        printf("SAT\n");
    else
        printf("UNSAT\n");

    fclose(file);

    return 0;
}