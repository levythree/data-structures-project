#ifndef priorityQueueH
#define priorityQueueH

typedef struct priorityQueueNode {
    void* data;
    struct priorityQueueNode *next;
} PriorityQueueNode;

typedef struct {
    PriorityQueueNode* head;
    int (*compare) (void*, void*);
} PriorityQueue;

PriorityQueueNode* createPriorityQueueNode(void* data);

PriorityQueue* createPriorityQueue(int (*compare) (void*, void*));

void enqueue(PriorityQueue* priorityQueue, void* data);

void* dequeue(PriorityQueue* priorityQueue);

#endif
