#include"Qeueu.c"
struct msgbuff
{
    long mtype; //used to determine reciver
    struct process_data pd;
};
int main(int argc, char * argv[])
{
    queue readyq;
    initialize(&readyq);
     int shmid;
         key_t key_id;
key_id = ftok("keyfile", 65);
shmid = shmget(key_id, 256, IPC_CREAT | 0644);
 union Semun semun;
    key_id = ftok("keyfile", 66);
    int sem1 = semget(key_id, 1, 0666 | IPC_CREAT);
    if (sem1 == -1)
    {
        perror("Error in create sem");
        exit(-1);
    }

    semun.val = 0; /* initial value of the semaphore, Binary semaphore */
    if (semctl(sem1, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }
    
void *shmaddr = shmat(shmid, (void *)0, 0);
down(sem1);
readyq.front=((struct node*)shmaddr);
readyq.rear=((struct node*)shmaddr);
    display(readyq.front);
  printf("In Shared Memory %d\n",(readyq.rear)->data.id);

    // initClk();

    // int rec_val, msgq_id;

    // key_id = ftok("keyfile", 65);               //create unique key
    // msgq_id = msgget(key_id, 0666 | IPC_CREAT); //create message queue and return id

    // if (msgq_id == -1)
    // {
    //     perror("Error in create");
    //     exit(-1);
    // }
    // printf("Message Queue ID = %d\n", msgq_id);

    // struct msgbuff message;
    // while(1)
    // {
    //     /* receive all types of messages */
    //     rec_val = msgrcv(msgq_id, &message, sizeof(message.pd), 0, !IPC_NOWAIT);

    //     if (rec_val == -1)
    //         perror("Error in receive");
    //     else
    //     {
    //      printf("\nMessage received: %d\n", message.pd.id);
    //         enqueue(readyq, message.pd)

    //     }
            
    // }

    // msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0);

    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    
    // destroyClk(true);
}
