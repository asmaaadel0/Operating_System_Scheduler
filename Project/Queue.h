#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include "headers.h"

typedef struct qNode
{
    PCB data;
    struct qNode *next;
} qNode;

typedef struct queue
{
    struct qNode *front, *rear;
    int size;
} queue;
qNode *newNode(PCB d);
queue initQueue();
void popQueue(queue *q);
PCB front(queue *q);
PCB rear(queue *q);
void pushQueue(queue *Q, PCB data);
int isEmptyQueue(queue *q);





int getsize(const queue *q);
void printqueue(const queue *q);

#endif
