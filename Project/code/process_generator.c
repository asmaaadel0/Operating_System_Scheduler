#include "headers.h"

void clearResources(int);

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    FILE *infile;
    int i = 0;
    char * str=malloc(10*sizeof(char));
    int data;
    //struct queuept = newQueue(100);
    infile = fopen("input.txt", "r");
    if (!infile)
    {
       perror("error in file");
    }////////////////////////////////////////////
    
    int total_no_of_processes=0;
    int Memory_Size=10;
    process_data * processes=(process_data *)malloc(Memory_Size*sizeof(struct process_data));//intial size
    //struct process_data dataStruct[3];
    char readline[1000];


    while (fgets(readline,1000,infile)!=NULL)//read from file
    {
        if(readline[0]=='#')
           continue;
        char * splitByTap=strtok(readline,"\t");//split
      
      processes[total_no_of_processes].id=atoi(splitByTap);
        for(int i=0;i<3;i++)
        {
            splitByTap=strtok(splitByTap,"\t");
        if(i==0)
            processes[total_no_of_processes].arrive=atoi(splitByTap);
        if(i==1)
            processes[total_no_of_processes].run_time=atoi(splitByTap);
        if(i==2)
            processes[total_no_of_processes].priority=atoi(splitByTap);
        }
        total_no_of_processes++;
        if(total_no_of_processes==Memory_Size-1)
        {
            Memory_Size=Memory_Size*2;
           struct process_data *new = realloc(processes, sizeof(struct process_data)* Memory_Size);//reallocation
            if(new==NULL)
            {
             perror("Error while reallocating memory\n");
            }
            processes = new;
         }
    }
    //allaocate  processes with no of processes
   struct process_data *new =realloc(processes, sizeof(struct process_data)*total_no_of_processes);//reallocation
    processes=new;
    fclose(infile);

    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
     int Algorithm=atoi(argv[1]);
     int quantum=-1;
     if(argc>2)//may thier is a quatum
         quantum=atoi(argv[2]);
    // 3. Initiate and create the scheduler and clock processes.
    int pid=fork();
    if(pid==0)
    {
    execl("clk.out","clk","-f",NULL);
    }else if(pid!=0)
    {
        //instainate schedular
         pid=fork();
         if(pid==0)
         {
            execl("sch.out","sch","-f",NULL);
         }

    }
    // 4. Use this function after creating the clock process to initialize clock

    // TODO Generation Main Loop

    // 5. Create a data structure for processes and provide it with its parameters.
    //DONE in reading
    
    // 6. Send the information to the scheduler at the appropriate
    int msgq_id, send_val;
    msgq_id = msgget(KEYFILE, 0666 | IPC_CREAT);
    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }

    //1.Send ALgorthim

    Algorithmmsgbuff Algorithm_to_Send;
    Algorithm_to_Send.mtype=10;
    Algorithm_to_Send.Algorithm=Algorithm;
    Algorithm_to_Send.quatum=quantum;//==>for Round Robin

    int send_value = msgsnd(msgq_id, &Algorithm_to_Send, sizeof(Algorithm_to_Send.Algorithm)+sizeof(Algorithm_to_Send.quatum),!IPC_NOWAIT);

    if (send_value == -1)
     perror("Errror in send");

    
    //2.send processes when their time
    
    initClk();
    int Processes_Index=0;

    struct processmsgbuff processmessage;
    processmessage.End=false;

    int index=0;//we reached indx in the processes[] 

    union Semun semun;
    int sem1 = semget(SEMFILE, 1, 0666 | IPC_CREAT);
      if (sem1 == -1 )
    {
        perror("Error in create sem");
        exit(-1);
    }
    semun.val = 0;
        if (semctl(sem1, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }

    while(1)
    {
        int No_Of_Processes_to_Send=0;
    // To get time use this
    down(sem1);
    int x = getClk(); 
    while(Processes_Index<total_no_of_processes&&processes[Processes_Index].arrive==x)//pop it
    {
        No_Of_Processes_to_Send++;
        Processes_Index++;

    }
    if(No_Of_Processes_to_Send==0)//no processes to send now
      { //send to him that NO send
      //send Message
           processmessage.mtype = 7; /* arbitrary value */     
           processmessage.Noofprocesses_to_Send=0;
           
         //  send_val = msgsnd(msgq_id, &processmessage, sizeof(processmessage.Noofprocesses_to_Send)+sizeof(processmessage.process)+sizeof(processmessage.End),!IPC_NOWAIT);
        send_val = msgsnd(msgq_id, &processmessage, sizeof(processmessage.Noofprocesses_to_Send),!IPC_NOWAIT);

           if (send_val == -1)
             perror("Errror in send");

      }
      else{
          while(No_Of_Processes_to_Send>0){
          //send Message
           processmessage.mtype = 7; /* arbitrary value */     
           processmessage.Noofprocesses_to_Send=No_Of_Processes_to_Send;///zayed wa7da
           processmessage.process=processes[index];
            
            if(index==total_no_of_processes-1)
              processmessage.End=true;


           send_val = msgsnd(msgq_id, &processmessage, sizeof(processmessage.Noofprocesses_to_Send)+sizeof(processmessage.process)+sizeof(processmessage.End),!IPC_NOWAIT);

           if (send_val == -1)
             perror("Errror in send");

           index++;
           No_Of_Processes_to_Send--;

          }


      }
    printf("current time is %d\n", x);
    
    }
    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}