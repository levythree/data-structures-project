#include <stdio.h>
#include <stdlib.h>

#include "priorityQueue.h"
#include "compare.h"

int main() {
    PriorityQueue* priorityQueue = createPriorityQueue(compareInt);

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

    return 0;
}
