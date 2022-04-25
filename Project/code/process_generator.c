//#include "headers.h"
#include"Qeueu.c"
void clearResources(int);
struct msgbuff
{
    long mtype; //used to determine reciver
    struct process_data pd;
};

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
        int numofProcesses=100;
        char Algorithm[10];
    // TODO Initialization
    // 1. Read the input files.
    FILE *infile;
    int i = 0;
    char *str=malloc(10*sizeof(char));
    int data;
    //struct queue *pt = newQueue(100);
    infile = fopen("input.txt", "r");
    if (!infile)
    {
        printf("error in file");
    }////////////////////////////////////////////
    while (i<4 && fscanf(infile, "%s", str) != EOF )
    {
        i++;

    }
    int j=0;
    ///process_data * dataStruct=malloc(numofProcesses*sizeof(process_data));
    struct process_data dataStruct[3];

    while (fscanf(infile,"%d",&data)!=EOF)
    {
        //process_data data1;
        dataStruct[j].id=data;
        fscanf(infile,"%d",&data);
        dataStruct[j].arrive=data;
        fscanf(infile,"%d",&data);
        dataStruct[j].run_time=data;
        fscanf(infile,"%d",&data);
        dataStruct[j].priority=data;
        j++;

    }
    int size=sizeof(dataStruct)/sizeof(dataStruct[0]);


    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    // scanf("%s",str);



    // 3. Initiate and create the scheduler and clock processes.
    int pid=fork();
    if(pid==0)
    {
    execl("clk.out","clk","-f",NULL);
    }
    //else{
      //  pid=fork();
       // if(pid==0){
         //   execl("scheduler.out","scheduler","-f",NULL);
       // }
   // }



    // 4. Use this function after creating the clock process to initialize clock

    initClk();
    // To get time use this
//printf("%d\n",x);

    union Semun semun;
     key_t key_id;
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

  j=0;
  int ck;
   queue tmp;
 initialize(&tmp);
 enqueue(&tmp,dataStruct[0]);
    // key_t key_id;
 int shmid;
key_id = ftok("keyfile", 65);
shmid = shmget(key_id, 256, IPC_CREAT | 0644);
void *shmaddr = shmat(shmid, (void *)0, 0);

((struct node*)shmaddr)->next=tmp.front;
((struct node*)shmaddr)[0]=*(tmp.rear);
up(sem1);

sleep(1000);

    //shmid = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0644);

//  while(j<size)
// {
//     ck = getClk();
//  while(j<size&&dataStruct[j].arrive<=ck)
//  {
     
//       key_t key_id;
//     int msgq_id, send_val;

//     key_id = ftok("keyfile", 65);
//     msgq_id = msgget(key_id, 0666 | IPC_CREAT);

//     if (msgq_id == -1)
//     {
//         perror("Error in create");
//         exit(-1);
//     }
//     // printf("Message Queue ID = %d\n", msgq_id);

//     // char str[] = "\nMessage from sender to receiver..\n";
//     struct msgbuff message;

//     message.mtype = 7; /* arbitrary value */
//     // strcpy(message.mtext, str);
//     message.pd=dataStruct[j];

//     send_val = msgsnd(msgq_id, &message, sizeof( message.pd), !IPC_NOWAIT);
    
//     if (send_val == -1)
//         perror("Errror in send");
//      j++;
//  }

//  }
    // printf("current time is %d\n", ck);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.

    // 6. Send the information to the scheduler at the appropriate time.

    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}
