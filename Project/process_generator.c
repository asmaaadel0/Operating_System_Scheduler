#include "headers.h"
#include "Queue.c"
void clearResources(int);
int msgq_id;
int sem1;
int send_val;
int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    //==========================================ipcs============================//
    // Messge queue process_generator and schedular
    msgq_id = msgget(KEYFILE, 0666 | IPC_CREAT);
    if (msgq_id == -1)
    {
        perror("Error in msgget()");
        exit(-1);
    }
    // semaphore between process_generator and clk
    sem1 = getsem(SEM1KEY);
    //============================================================================//
    // TODO Initialization
    // 1. Read the input files.
    FILE *infile;
    int i = 0;
    char *str = malloc(10 * sizeof(char));
    int data;
    infile = fopen("processes.txt", "r");
    if (!infile)
    {
        perror("error in file");
    }
    // 2. Create a data structure for processes and provide it with its parameters.
    queue processes_info = initQueue(); // Queue data structue
    char line[100];
    char delim[] = "    ";
    PCB process;
    while (fgets(line, sizeof(line), infile) != NULL)
    {
        if (line[0] == '#') // skip it
            continue;
        char *token = strtok(line, delim); // split line read
        process.process_id = atoi(token);

        for (int i = 0; i < 4; i++)
        {
            token = strtok(NULL, delim);
            if (i == 0)
                process.arrival_time = atoi(token);
            else if (i == 1)
                process.excution_time = atoi(token);
            else if (i == 2)
                process.priority = atoi(token);

			else if(i==3)
               process.memsize=atoi(token);
        }
           
        // push it in the queue
        pushQueue(&processes_info, process);
    }
    fclose(infile);
    //=====================================================================================//
    // 3. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    int quantum = -1;
    int algorithm_number = -1;
    printf("Please enter : \n[0] for HPF\n[1] for SRTN\n[2] for RR\n");
    scanf("%d", &algorithm_number);
    if (algorithm_number == 2)
    {
        printf("Please enter the required quantum for RR : \n");
        scanf("%d", &quantum);
    }

    //====================================================================================//
    // 4. Initiate and create the scheduler and clock processes.
    int pid = fork();
    // clock
    if (pid == 0)
    {
        execl("clk.out", "clk", NULL);
    }
    else if (pid == -1)
    {
        perror("Error in fork\n");
        exit(-1);
    }
    else if (pid != 0)
    {
        // schedular
        pid = fork();
        if (pid == 0)
        {
            execl("scheduler.out", "scheduler", "-f", NULL);
        }
        else if (pid == -1)
        {
            perror("Error in fork\n");
            exit(-1);
        }
    }
    //============================================================================================//
    // 5. send algorithm num to scheduler and Quantum
    Algorithmmsgbuff Algorithm_to_Send;
    Algorithm_to_Send.mtype = 10;
    Algorithm_to_Send.Algorithm = algorithm_number;
    Algorithm_to_Send.quantum = quantum; //==>for Round Robin

    int send_value = msgsnd(msgq_id, &Algorithm_to_Send, sizeof(Algorithm_to_Send.Algorithm) + sizeof(Algorithm_to_Send.quantum), !IPC_NOWAIT);
        perror("Errror in send");
    //==========================================================================================//
    // 6. Use this function after creating the clock process to initialize clock
    initClk();
    //========================================================================================//

    //**************************************************************************//
    /////////////////////////////start main loop//////////////////////////////////
    while (1)
    {
        down(sem1);
        int clk = getClk();
        printf("process_genrator: Clk is%d\n", clk);
        // // TODO Generation Main Loop
        processmsgbuff processmessage;
        ///////////////////////////start send loop/////////////////////////////////
        // // 7. Send the information to the scheduler at the appropriate time.
        while (processes_info.front != NULL && processes_info.front->data.arrival_time <= clk) // the processes whose arrival time is x(now)==>time to send them to the schedular
        {
            PCB frontprocess = front(&processes_info);
            popQueue(&processes_info);
            processmessage.mtype = 7;

            // 1.process info to be sent
            processmessage.process.process_id = frontprocess.process_id;
            processmessage.process.arrival_time = frontprocess.arrival_time;
            processmessage.process.excution_time = frontprocess.excution_time;
            processmessage.process.priority = frontprocess.priority;
			processmessage.process.memsize = frontprocess.memsize;

            // 2.NO of processes to sent 1 or 0
            processmessage.End = false;
            processmessage.Noofprocesses_to_Send = 1;
            // send
            send_val = msgsnd(msgq_id, &processmessage, sizeof(processmessage.Noofprocesses_to_Send) + sizeof(processmessage.process) + sizeof(processmessage.End), !IPC_NOWAIT);
            if (send_val == -1)
                perror("Errror in send");
        }
        //No processes to send NOW!!
        if (processes_info.front != NULL)
        {
            processmessage.mtype = 7;
            processmessage.Noofprocesses_to_Send = 0;
            processmessage.End = false;
            send_val = msgsnd(msgq_id, &processmessage, sizeof(processmessage.Noofprocesses_to_Send) + sizeof(processmessage.process) + sizeof(processmessage.End), !IPC_NOWAIT);
            if (send_val == -1)
                perror("Errror in send");
        }
        //NO more proesses to read!!
        else
        {
            processmessage.mtype = 7;
            processmessage.Noofprocesses_to_Send = 0;
            processmessage.End = true;
            send_val = msgsnd(msgq_id, &processmessage, sizeof(processmessage.Noofprocesses_to_Send) + sizeof(processmessage.process) + sizeof(processmessage.End), !IPC_NOWAIT);
            if (send_val == -1)
                perror("Errror in send");
            // No further processes
            break;
        }
        ///////////////////////////end send loop/////////////////////////////////
        //===================================================================================//
    }
    ///////////////////////////////end main loop///////////////////////////////////
    //**************************************************************************//

    //========================================================================================//

    // wait for the schedular to termenaite
    int stat_loc;
    int pid2 = waitpid(pid, &stat_loc, 0);
    printf(" pid %d   PROCESS GERNATOR WILL TERMINATE  \n", pid2);



    // 8. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    int val;
    //message queue between process_generator and schedular
    val = msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0);
    if (val == -1)
    {
        perror("Error in ctrl\n");
        exit(-1);
    }
    //message queue between process and schedular
    int msgq_pro_sch_id = msgget(KEYFILE_pro_sch, 0666 | IPC_CREAT);
    if (msgq_pro_sch_id == -1)
    {
        perror("Error in msgget()");
        exit(-1);
    }
    val = msgctl(msgq_pro_sch_id, IPC_RMID, (struct msqid_ds *)0);
    if (val == -1)
    {
        perror("Error in ctrl\n");
        exit(-1);
    }
    //semaphore between clk and schedular
    val = semctl(sem1, 0, IPC_RMID, semun);
    if (val == -1)
    {
        perror("Error in ctrl\n");
        exit(-1);
    }
    //semaphore between clk and process_generator
    int sem2 = semget(SEM2KEY, 1, 0666 | IPC_CREAT);
    val = semctl(sem2, 0, IPC_RMID, semun);
    if (val == -1)
    {
        perror("Error in ctrl\n");
        exit(-1);
    }
    raise(SIGKILL);
}
