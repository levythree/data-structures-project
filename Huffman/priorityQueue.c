#include <stdlib.h>

#include "priorityQueue.h"

PriorityQueueNode* createPriorityQueueNode(void* data) {
    PriorityQueueNode* priorityQueueNode = (PriorityQueueNode*) malloc(sizeof(PriorityQueueNode));

    priorityQueueNode->data = data;
    priorityQueueNode->next = NULL;

    return priorityQueueNode;
}

PriorityQueue* createPriorityQueue(int (*compare) (void*, void*)) {
    PriorityQueue* priorityQueue = (PriorityQueue*) malloc(sizeof(PriorityQueue));

    priorityQueue->head = NULL;
    priorityQueue->compare = compare;

    return priorityQueue;
}

void enqueue(PriorityQueue* priorityQueue, void* data) {
    PriorityQueueNode* priorityQueueNode = createPriorityQueueNode(data);

    if (priorityQueue->head == NULL) priorityQueue->head = priorityQueueNode;
    else {
        PriorityQueueNode* previous = NULL;
        PriorityQueueNode* current = priorityQueue->head;

        while (current != NULL && priorityQueue->compare(data, current->data) > 0) {
            previous = current;
            current = current->next;
        }

        if (previous == NULL) {
            priorityQueueNode->next = current;
            priorityQueue->head = priorityQueueNode;
        } else {
            previous->next = priorityQueueNode;
            priorityQueueNode->next = current;
        }
    }
}

void* dequeue(PriorityQueue* priorityQueue) {
    if (priorityQueue->head == NULL) return NULL;

    PriorityQueueNode* removedNode = priorityQueue->head;

    void* removedData = removedNode->data;

    priorityQueue->head = removedNode->next;

    free(removedNode);

    return removedData;
}
