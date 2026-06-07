#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "priorityQueue.h"
#include "compare.h"
#include "huffman.h"
#include "fileHandle.h"

int main(int numberOfArguments, char** arguments) {
    if (numberOfArguments != 4) {
        printf("Usage: ./main <compress>|<decompress> <inputFileName> <outputFileName>\n");

        return 1;
    }

    char* command = arguments[1];
    char* inputFileName = arguments[2];
    char* outputFileName = arguments[3];

    if (strcmp(command, "compress") == 0) compressFile(inputFileName, outputFileName);
    else if (strcmp(command, "decompress") == 0) decompressFile(inputFileName, outputFileName);
    else printf("Invalid command.\n");

    return 0;
}
