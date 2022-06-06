#ifndef SORTEDLINKEDLIST_H
#define SORTEDLINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>
//sorted linked list of int
#include <math.h>

struct nodesorted {
    int data;//start index
    struct nodesorted *next;
};
typedef struct nodesorted NODE;
 
struct linkedList {
    NODE *head;
    int size;
};

/* linked list functions */
struct linkedList * creatLinkedList();
int isempty(struct linkedList * ll);
void InsertOrdered(struct linkedList *ll, int );
int DeleteNode(struct linkedList *ll, int );
int findNode(struct linkedList* ll, int data);
void Traverse(struct linkedList *ll);

void print_All_Empty_Lists(int size,struct linkedList* free_Lists[]);

#endif