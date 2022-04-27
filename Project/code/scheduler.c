//#include "headers.h"
#include "queue.c"


int main(int argc, char * argv[])
{
    initClk();
    
    //TODO implement the scheduler :)
    //upon termination release the clock resources.

    //Message buffer
    int msgq_id, rec_val;
    msgq_id = msgget(KEYFILE, 0666 | IPC_CREAT);

    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    struct processmsgbuff processmessage;
    //1.Recieve algorthim

     Algorithmmsgbuff algorithm;
      rec_val = msgrcv(msgq_id, &algorithm,sizeof(algorithm.Algorithm)+sizeof(algorithm.quatum),10, !IPC_NOWAIT);

        if (rec_val == -1)
            perror("Error in receive");




    //2.Recieve from Queue
    int No_Of_Pocesses_to_Recieve;
    bool End=false;
    process_data process_to_Recieve;

    queue ready_Queue;///this depends on Algo
    ready_Queue=initQueue();

//loop of the scedular 
    while(!End||!isEmptyQueue(&ready_Queue))///DON'T FORGET to Add your DS to this while ||
    {
        //1.recieve
       do{

            rec_val = msgrcv(msgq_id, &processmessage,sizeof(processmessage.Noofprocesses_to_Send)+sizeof(processmessage.process)+sizeof(processmessage.End),7, !IPC_NOWAIT);

        if (rec_val == -1)
            perror("Error in receive");
        else{

            No_Of_Pocesses_to_Recieve=processmessage.Noofprocesses_to_Send;
            if(No_Of_Pocesses_to_Recieve==0)//no processes to recive ==> break
                break;

            End=processmessage.End;
            process_to_Recieve=processmessage.process;

            //Add this to the ready Queue
            pushQueue(&ready_Queue,process_to_Recieve);//////this depends on Algo
            }

       }while(No_Of_Pocesses_to_Recieve>1);


       //2.Assigning Scheduling algorithms


       printf("Algorthim: %d    quantum:%d\n",algorithm.Algorithm,algorithm.quatum);
       for(int i=0;i<ready_Queue.size;i++)
       {
            process_data p=front(&ready_Queue);
            popQueue(&ready_Queue);
            printf("%d\n",p.id);

       }


    }
    
    destroyClk(true);
}
