#ifndef Q_H
#define Q_H

#include <stdio.h>
#include <stdlib.h>
#include "headers.h"

typedef struct qNode
{
    process_data data;
    struct qNode *next;
} qNode;

typedef struct queue
{
    struct qNode *front, *rear;
    int size;
} queue;
qNode *newNode(process_data d);
queue initQueue();
void popQueue(queue *q);
process_data front(queue *q);
process_data rear(queue *q);
void pushQueue(queue *Q, process_data data);
int isEmptyQueue(queue *q);

#endif