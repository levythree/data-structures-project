#include <stdio.h>
#include <stdlib.h>

#include "priorityQueue.h"
#include "compare.h"
#include "huffman.h"

int main() {
    PriorityQueue* priorityQueue = createPriorityQueue(compareHuffmanNodes);

    /*
    int i;

    for (i = 0; i < 10; i++) {
        int* value = (int*) malloc(sizeof(int));

        scanf("%d", value);

        enqueue(priorityQueue, (void*) value);
    }

    PriorityQueueNode* current = priorityQueue->head;

    while (current != NULL) {
        printf("%d ", *(int*) current->data);

        current = current->next;
    }

    void* data1 = dequeue(priorityQueue);
    void* data2 = dequeue(priorityQueue);
    void* data3 = dequeue(priorityQueue);
    
    printf("\n%d %d %d\n", *(int*) data1, *(int*) data2, *(int*) data3);

    current = priorityQueue->head;

    while (current != NULL) {
        printf("%d ", *(int*) current->data);

        current = current->next;
    }
    */

    void* huff1 = createHuffmanNode('D', 3);
    void* huff2 = createHuffmanNode('B', 5);
    void* huff3 = createHuffmanNode('A', 6);
    void* huff4 = createHuffmanNode('F', 1);
    void* huff5 = createHuffmanNode('E', 2);
    void* huff6 = createHuffmanNode('C', 4);

    enqueue(priorityQueue, huff1);
    enqueue(priorityQueue, huff2);
    enqueue(priorityQueue, huff3);
    enqueue(priorityQueue, huff4);
    enqueue(priorityQueue, huff5);
    enqueue(priorityQueue, huff6);

    HuffmanNode* root = createHuffmanTree(priorityQueue);

    printPreOrderFrequencies(root);

    printf("\n");

    printPreOrderCharacters(root);

    printf("\n");

    HuffmanCode* code = (HuffmanCode*) malloc(256 * sizeof(HuffmanCode));

    int i, j;

    for (i = 0; i < 256; i++) code[i].length = 0;

    char* path = (char*) malloc(256 * sizeof(char));

    generateDictionary(root, path, 0, code);

    for (i = 0; i < 256; i++) {
        if (code[i].length > 0) {
            printf("%c: ", i);

            for (j = 0; j < code[i].length; j++) printf("%c", code[i].bits[j]);

            printf("\n");
        }
    }

    return 0;
}
