#include "queue.h"
qNode *newNode(process_data d)
{
    qNode *temp = (qNode *)malloc(sizeof(qNode));
    temp->data = d;
    temp->next = NULL;
    return temp;
}

queue initQueue()
{
    queue q;
    q.front = q.rear = NULL;
    q.size = 0;
    return q;
}

void pushQueue(queue *q, process_data k)
{
    qNode *temp = newNode(k);
    if (q->rear == NULL)
    {
        q->front = q->rear = temp;
        q->size = q->size + 1;
        return;
    }
    q->rear->next = temp;
    q->rear = temp;
    q->size = q->size + 1;
}
void popQueue(queue *q)
{
    if (q->front == NULL)
        return;
    qNode *temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL)
        q->rear = NULL;

    free(temp);
    q->size = q->size - 1;
}
process_data front(queue *q)
{

    if (q->front != NULL)
        return q->front->data;

    process_data null;
    null.id=-1; 
    return null;
};

process_data rear(queue *q)
{

    if (q->rear != NULL)
        return q->rear->data;
    process_data null;
    null.id=-1; 
    return null;
};

int isEmptyQueue(queue *q)
{
    return q->size == 0;
}