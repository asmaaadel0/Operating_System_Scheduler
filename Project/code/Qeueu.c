#include <stdio.h>
#include <stdlib.h>
#include "headers.h"
#define TRUE 1
#define FALSE 0
#define FULL 100

struct node
{
    struct process_data data;
    struct node *next;
};
typedef struct node node;

struct queue
{
    int count;
    node *front;
    node *rear;
};
typedef struct queue queue;

void initialize(queue *q)
{
    q->count = 0;
    q->front = NULL;
    q->rear = NULL;
}

int isempty(queue *q)
{
    return (q->rear == NULL);
}

void enqueue(queue *q, struct process_data value)
{
    if (q->count < FULL)
    {
        node *tmp;
        tmp = malloc(sizeof(node));
        tmp->data = value;
        tmp->next = NULL;
        if(!isempty(q))
        {
            q->rear->next = tmp;
            q->rear = tmp;
        }
        else
        {
            q->front = q->rear = tmp;
        }
        q->count++;
    }
    else
    {
        printf("List is full\n");
    }
}

struct process_data dequeue(queue *q)
{
    node *tmp;
    struct process_data n = q->front->data;
    tmp = q->front;
    q->front = q->front->next;
    q->count--;
    free(tmp);
    return(n);
}

// void display(node *head)
// {
//     if(head == NULL)
//     {
//         printf("NULL\n");
//     }
//     else
//     {
//         printf("%d\n", head -> data);
//         display(head->next);
//     }
// }