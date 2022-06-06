#include "Queue.h"
qNode *newNode(PCB d)
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

void pushQueue(queue *q, PCB k)
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
PCB front(queue *q)
{

    if (q->front != NULL)
        return q->front->data;

    PCB null;
    null.process_id = -1;
    return null;
};

PCB rear(queue *q)
{

    if (q->rear != NULL)
        return q->rear->data;
    PCB null;
    null.process_id = -1;
    return null;
};

int isEmptyQueue(queue *q)
{
    return q->size == 0;
}

int getsize(const queue *q)
{
    return q->size;
}

void printqueue(const queue *q)
{
    if (q == NULL)
        return;
    qNode *temp = q->front;
    printf("Queue:\n");
    while (temp != NULL)
    {
        char state[10];
        if (temp->data.state == waiting)
            strcpy(state, "waiting");
        else if (temp->data.state == running)
            strcpy(state, "running");
        else strcpy(state, ".........");

        printf("id:%d System_id:%d State:%s ArrivaTime:%d ExecutionTime:%d Priority:%d WaitingTime:%d StartTime:%d CumRunningTime:%d RemainingTime:%d\n", temp->data.process_id, temp->data.process_system_id, state, temp->data.arrival_time, temp->data.excution_time, temp->data.priority, temp->data.waiting_time, temp->data.start_time, temp->data.cumulative_running_time, temp->data.remaining_time);
        temp = temp->next;
    }
}