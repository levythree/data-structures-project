#ifndef parserH
#define parserH

#include <stdio.h>
#include <stdbool.h>

#include "ast.h"

bool lexerNextToken(FILE* file, char* token);

int lexerPeekChar(FILE* file);

NodeType getNodeType(char* token);

ASTNode* binaryTreeParser(FILE* file);

void parseSMTFile(FILE* file, ASTNode** array, int* counter);

#endif