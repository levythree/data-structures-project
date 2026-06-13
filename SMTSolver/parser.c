#include "./types/parser.h"

#include <ctype.h>
#include <string.h>
#include <stdbool.h>

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

NodeType getNodeType(char* token) {
    if (!strcmp(token, "+")) return NODE_OP_ADD;
    if (!strcmp(token, "-")) return NODE_OP_SUB;
    if (!strcmp(token, "=")) return NODE_OP_EQ;
    if (!strcmp(token, "<=")) return NODE_OP_LE;
    if (!strcmp(token, ">=")) return NODE_OP_GE;
    if (!strcmp(token, "*")) return NODE_OP_MUL;
    if (!strcmp(token, "or")) return NODE_OP_OR;

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

    if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1])))
        return createNode(NODE_NUM, token);
    else
        return createNode(NODE_VAR, token);
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

            } else if (strcmp(command, "assert") == 0) {
                ASTNode* tree = binaryTreeParser(file);
                array[(*counter)++] = tree;

                lexerNextToken(file, token);

            } else if (strcmp(command, "check-sat") == 0) {
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
