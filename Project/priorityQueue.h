#ifndef PRIORIYQ_H

#define PRIORIYQ_H

#include "headers.h"
#include <stdio.h>
#include <stdlib.h>

//Node
typedef struct Node {
	PCB data;

	// Lower values indicate higher priority
	int priority;

	struct Node* next;

} Node;

typedef struct minHeap
{    
    int size;
    Node *head;
} minHeap;




//Intialize
minHeap initMinHeap();
Node* pqnewNode(PCB d, int p);
void push(minHeap* pq, int p, PCB d);  //p==>priortiy;
PCB peek(minHeap* pq);
void pop(minHeap* pq);
int isEmpty(minHeap* pq);
void print(minHeap pq);
#endif