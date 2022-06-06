#include "priorityQueue.h"

// Function to Create A New Node
Node* pqnewNode(PCB d, int p)
{
	Node* temp = (Node*)malloc(sizeof(Node));
	temp->data = d;
	temp->priority = p;
	temp->next = NULL;

	return temp;
}

//Intialize
minHeap initMinHeap()
{
    
    minHeap pq;
    pq.head=NULL;
    pq.size = 0;
    return pq;
}

// Function to push according to priority
void push(minHeap* pq, int p, PCB d)  //p==>priortiy
{
    if(pq==NULL)
    {
        printf("NULL Priorty queue\n");
        exit(-1);
    }
	Node* start = (pq->head);
    //first node
    if(start==NULL)
    {
        //create head
        Node* head = pqnewNode(d, p);
        pq->head=head;
        pq->size++;
        return;
    }

	// Create new Node
	Node* temp = pqnewNode(d, p);

	// Special Case: The head of list has lesser
	// priority than new node. So insert new
	// node before head node and change head node.
	if ((pq->head)->priority > p) {

		// Insert New Node before head
		temp->next = pq->head;
		pq->head = temp;
	}
	else {

		// Traverse the list and find a
		// position to insert new node
		while (start->next != NULL &&
			start->next->priority <= p) {
			start = start->next;
		}

		// Either at the ends of the list
		// or at required position
		temp->next = start->next;
		start->next = temp;
	}
}



// Return the value at head
PCB peek(minHeap* pq)
{
    if(pq==NULL)
    {
        printf("NULL Priorty queue\n");
        exit(-1);
    }
    if(pq->head==NULL)
    {
        PCB null;
        null.process_id = -1;
        return null;
    }
	return pq->head->data;
}

// Removes the element with the
// highest priority form the list
void pop(minHeap* pq)
{
    if(pq==NULL)
    {
        printf("NULL Priorty queue\n");
        exit(-1);
    }
	Node* temp = pq->head;
	(pq->head) = (pq->head)->next;
    pq->size--;
	free(temp);
}


// Function to check is list is empty
int isEmpty(minHeap* pq)
{
    if(pq==NULL)
    {
        printf("NULL Priorty queue\n");
        exit(-1);
    }
	return pq->head == NULL;
}

void print(minHeap pq)
{
	while (!isEmpty(&pq)) {
        PCB  peekpcb=peek(&pq);


         char state[10];
        if (peekpcb.state == waiting)
            strcpy(state, "waiting");
        else if (peekpcb.state == running)
            strcpy(state, "running");
        else strcpy(state, ".........");

        printf("id:%d System_id:%d State:%s ArrivaTime:%d ExecutionTime:%d Priority:%d WaitingTime:%d StartTime:%d CumRunningTime:%d RemainingTime:%d\n", peekpcb.process_id,peekpcb.process_system_id, state, peekpcb.arrival_time,peekpcb.excution_time, peekpcb.priority,peekpcb.waiting_time, peekpcb.start_time,peekpcb.cumulative_running_time,peekpcb.remaining_time);
		pop(&pq);
	}
}

// // Driver code
// int main()
// {

// minHeap pq=initMinHeap();


//     PCB a1;
//     a1.arrival_time=1;
//     a1.mem_end_index=1;



//         PCB a2;
//     a2.arrival_time=1;   
//     a2.mem_end_index=2;


//         PCB a3;
//     a3.arrival_time=1;   
//     a3.mem_end_index=3;


//     PCB a4;
//     a4.arrival_time=1;   
//     a4.mem_end_index=4;


//     push(&pq,a1.arrival_time,a1);
//      push(&pq,a2.arrival_time,a2);
//       push(&pq,a3.arrival_time,a3);
//        push(&pq,a4.arrival_time,a4);


//        print(pq);

// 	// // Create a Priority Queue
// 	// // 7->4->5->6

// 	// Node* pq = newNode(a1, a1.arrival_time);
// 	// push(&pq, a2,a2.arrival_time );
// 	// push(&pq, a3, a3.arrival_time);
// 	// push(&pq, a4, a4.arrival_time);

// printf("hoiiiiii\n");
// 	while (!isEmpty(&pq)) {
// 		printf("%d ", peek(&pq).mem_end_index);
// 		pop(&pq);
// 	}

// 	return 0;
// }
