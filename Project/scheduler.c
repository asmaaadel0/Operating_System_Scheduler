#include "headers.h"
#include "Queue.c"
#include "priorityQueue.c"
#include "hashmap.c"
#include "buddy.c"
// #include <semaphore.h>

void handler_USR1(int sig_num);

struct hashmap *process_table;

terminated_process terminated[100];
int j = 0;

// ready queues for each algorithm
minHeap ready_Queue_HPF;
minHeap ready_Queue_SRTF;
queue ready_Queue_RR;

//Waiting Queue
minHeap Waiting_Queue_HPF;
minHeap tempQueue;

// functions for each algorithm
void HPF_Algorithm();
void SRTF_Algorithm();
void RR_Algorithm();

int Algorithm_num;
int quantum_num;

PCB *runingProcess = NULL;
PCB process_running;
int clk;
int rec_val, send_val;
int sem2, sem3;
int pid;
int msgq_pro_sch_id;
int i = 0;

// HPF
bool if_process_running = false;

// SRTN
void run_process();
void preemptProcess();
bool one_process_arrived = false;

// RR
int if_quantum_finish = 0;

//waiting  list  ///p0    p1    p5   (p8)
// Memory ==> linked list  //p8     p5
//zeinab&&asmaa2
int *shmaddr2;
////////////////////////////////calculations//////////////////////////////////////
FILE *scheduler_log;
FILE *scheduler_pref;
FILE *memory_log;

int all_process = 0;
int TA_sum = 0;
float WTA_sum = 0;
float standard_avg = 0;
int Num_Of_Processes = 0;

void print_in_scheduler_log(int state, int time, int TA, float WTA, PCB *runingProcess,int waitTime);
void print_memory_log(int clk, int memsize, int process_ID, int start_index, int end_index, int choice);


bool tempS=false;
//////////////////////////////hashmap functions///////////////////////////////////////
uint64_t pcb_hash(const void *item, uint64_t seed0, uint64_t seed1)
{
    const struct PCB *pcb = item;
    return hashmap_sip(&pcb->process_system_id, sizeof(int), seed0, seed1);
}

int pcb_compare(const void *a, const void *b, void *udata)
{
    const struct PCB *ua = a;
    const struct PCB *ub = b;
    if (ua->process_system_id > ub->process_system_id)
        return 1;
    else if (ua->process_system_id < ub->process_system_id)
        return -1;
    else
        return 0;
}

bool pcb_iter(const void *item, void *udata)
{
    const struct PCB *pcb = item;
    char str[10];
    if (pcb->state == 0)
        strcpy(str, "waiting");
    else
        strcpy(str, "running");
    printf("%d    %d           %s    %d         %d     %d     %d     %d    %d   %d\n", pcb->process_id, pcb->priority, str, pcb->start_time, pcb->excution_time, pcb->waiting_time, pcb->remaining_time, pcb->cumulative_running_time, pcb->process_system_id, pcb->memsize);
    return true;
}

////////////////////////////////main function//////////////////////////////
int main(int argc, char *argv[])
{
    signal(SIGUSR1, handler_USR1);
    //////////////////////////output file//////////////////////////////////
    // 1.open output file
    scheduler_log = fopen("scheduler.log", "w");
    fprintf(scheduler_log, "#At\tTime\tX\tProcess\tY\tState\tArrived\tW\tTotal\tZ\tRemain\tY\tWait\tK\n");
    scheduler_pref = fopen("scheduler.perf", "w");

    memory_log = fopen("memory.log", "w");
    fprintf(memory_log, "#At\tTime\tX\tallocated\tY\tbytes\tfor\tprocess\tZ\tfrom\ti\tto\tj\n");
    // fclose(scheduler_log);
    //////////////////////////////////////////////////////////////////////

    // create process table
    process_table = hashmap_new(sizeof(struct PCB), 0, 0, 0, pcb_hash, pcb_compare, NULL, NULL);

    /////////////////////////////////////ipcs///////////////////////////////
    // Messge queue
    int msgq_id;
    msgq_id = msgget(KEYFILE, 0666 | IPC_CREAT);
    if (msgq_id == -1)
    {
        perror("Error in msgget()");
        exit(-1);
    }
    msgq_pro_sch_id = msgget(KEYFILE_pro_sch, 0666 | IPC_CREAT);
    if (msgq_pro_sch_id == -1)
    {
        perror("Error in msgget()");
        exit(-1);
    }
    sem2 = getsem(SEM2KEY);
    sem3 = getsem(SEM3KEY);
    up(sem3);
    int y = semctl(sem3, 0, GETVAL);
    if (y == -1)
    {
        printf("%d", errno);
        printf("Error");
        exit(-1);
    }
    printf("\n***************************** semaphore  in scheduler at beginning /////////up///// %d *********************\n", y);

    //zeinab&&asmaa2
    int shmid2;
    shmid2 = shmget(SHKEY2, 256, IPC_CREAT | 0644);
    if (shmid2 == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    shmaddr2 = (int *)shmat(shmid2, (void *)0, 0);

    ///////////////////////////////////////////////////////////////////////

    // 2.recieve algorithm number and quantum
    Algorithmmsgbuff algorithm;
    rec_val = msgrcv(msgq_id, &algorithm, sizeof(algorithm.Algorithm) + sizeof(algorithm.quantum), 10, !IPC_NOWAIT);
    if (rec_val == -1)
        perror("Error in receive");

    Algorithm_num = algorithm.Algorithm;
    quantum_num = algorithm.quantum;
    if_quantum_finish = quantum_num;
    printf("\nquantum recive%d\n", quantum_num);

    // initialize ready queues
    //initilaize memory Waiting queue
    if (Algorithm_num == 0)
    {
        ready_Queue_HPF = initMinHeap();
    }
    else if (Algorithm_num == 1)
    {
        ready_Queue_SRTF = initMinHeap();
    }
    else if (Algorithm_num == 2)
    {
        ready_Queue_RR = initQueue();

        // Waiting_Queue_RR = initQueue();
    }
    Waiting_Queue_HPF = initMinHeap();
    tempQueue=initMinHeap();
    //intiliaze free lists
    intialize_free_lists();

    // initialize clk
    initClk();

    bool end = false;

    ///////////////////////////////////main loop////////////////////////////////////////
    while (!end || !isEmpty(&ready_Queue_SRTF) || runingProcess != NULL || !isEmpty(&ready_Queue_HPF) || !isEmptyQueue(&ready_Queue_RR))
    {
        down(sem2);
        int value;
        int y = semctl(sem3, 0, GETVAL);
        if (y == -1)
        {
            printf("%d", errno);
            printf("Error");
            exit(-1);
        }
        printf("\n***************************** semaphore  in scheduler outer loop /////////before down///// %d *********************\n", y);

        down(sem3);
        y = semctl(sem3, 0, GETVAL);
        if (y == -1)
        {
            printf("%d", errno);
            printf("Error");
            exit(-1);
        }
        printf("\n***************************** semaphore  in scheduler outer loop /////////down///// %d *********************\n", y);
        clk = getClk();
        printf("Sch:Clk is %d\n", clk);
        //==========================================================================//
        // 1.recieve the process whose time is now from the process_genrator
        int Noofprocessestorecieve = 1;
        processmsgbuff processmessage;
        PCB processrecieved;

        while (!end)
        {
            printf(",,,,,,,,,,,,,,,,\n");
            rec_val = msgrcv(msgq_id, &processmessage, sizeof(processmessage.Noofprocesses_to_Send) + sizeof(processmessage.process) + sizeof(processmessage.End), 7, !IPC_NOWAIT);
            if (rec_val == -1)
                perror("Error in receive");

            Noofprocessestorecieve = processmessage.Noofprocesses_to_Send;
            end = processmessage.End;
            if (end)
                break;
            if (Noofprocessestorecieve == 0)
                break;

            // fork process
            pid = fork();
            if (pid == 0)
            {
                execl("process.out", "process", NULL);
            }
            else if (pid == -1)
            {
                perror("Error in fork\n");
                exit(-1);
            }

            // insert process in process table
            processrecieved.process_id = processmessage.process.process_id;
            processrecieved.arrival_time = processmessage.process.arrival_time;
            processrecieved.excution_time = processmessage.process.excution_time;
            processrecieved.priority = processmessage.process.priority;
            processrecieved.memsize = processmessage.process.memsize;

            processrecieved.state = waiting;
            processrecieved.waiting_time = 0;
            processrecieved.remaining_time = processrecieved.excution_time;
            processrecieved.cumulative_running_time = 0;
            processrecieved.process_system_id = pid;
            processrecieved.start_time = -1;
            if (Algorithm_num == 0)
                { //HPF
                    push(&tempQueue, processrecieved.priority, processrecieved);
                }
                else if (Algorithm_num == 1)
                { //SRTF
                    push(&tempQueue, processrecieved.remaining_time, processrecieved);
                }
                else if (Algorithm_num == 2)
                {
                    push(&tempQueue, processrecieved.arrival_time, processrecieved);
                    // pushQueue(&Waiting_Queue_RR, processrecieved);
                }

            

            //  hashmap_scan(process_table, pcb_iter, NULL);
            Num_Of_Processes++;
        }
        //Allocate Mmemory for this process
        if(tempS){
            while(!isEmpty(&Waiting_Queue_HPF)){

                PCB temps=peek(&Waiting_Queue_HPF);
                pop(&Waiting_Queue_HPF);
                if (Algorithm_num == 0)
                { //HPF
                    push(&tempQueue, temps.priority, temps);
                }
                else if (Algorithm_num == 1)
                { //SRTF
                    push(&tempQueue, temps.remaining_time, temps);
                }
                else if (Algorithm_num == 2)
                {
                    push(&tempQueue, temps.arrival_time, temps);
                    // pushQueue(&Waiting_Queue_RR, processrecieved);
                }

            }
            tempS=false;
        }
        while(!isEmpty(&tempQueue)){
            PCB tempPCB=peek(&tempQueue);
            pop(&tempQueue);
            int mem_end_index;
            int mem_start_index = allocate(tempPCB.memsize, &mem_end_index);
            if (mem_start_index == -1)
            { //no space for it
                //Memory
                tempPCB.mem_start_index = -1;
                tempPCB.mem_end_index = -1;
                //add in the waiting queue
                if (Algorithm_num == 0)
                { //HPF
                    push(&Waiting_Queue_HPF, tempPCB.priority, tempPCB);
                }
                else if (Algorithm_num == 1)
                { //SRTF
                    push(&Waiting_Queue_HPF, tempPCB.remaining_time, tempPCB);
                }
                else if (Algorithm_num == 2)
                {
                    push(&Waiting_Queue_HPF, tempPCB.arrival_time, tempPCB);
                    // pushQueue(&Waiting_Queue_RR, processrecieved);
                }
            }
            else
            {
                tempPCB.mem_start_index = mem_start_index;
                tempPCB.mem_end_index = mem_end_index;

                //Print Memory.log
                // Num_Of_Processes++; //////////////////////////////
                // add it to the ready queue
                if (Algorithm_num == 0)
                {
                    push(&ready_Queue_HPF, tempPCB.priority, tempPCB);
                }
                else if (Algorithm_num == 1)
                {
                    push(&ready_Queue_SRTF, tempPCB.remaining_time, tempPCB);
                    one_process_arrived = true;
                }
                else if (Algorithm_num == 2)
                {
                    pushQueue(&ready_Queue_RR, tempPCB);
                }
                print_memory_log(clk, tempPCB.memsize, tempPCB.process_id, tempPCB.mem_start_index, tempPCB.mem_end_index, 1);
            }
              hashmap_set(process_table, &(struct PCB){.process_id = tempPCB.process_id, .priority = tempPCB.priority, .excution_time = tempPCB.excution_time, .waiting_time = tempPCB.waiting_time, .remaining_time = tempPCB.remaining_time, .cumulative_running_time = tempPCB.cumulative_running_time, .process_system_id = tempPCB.process_system_id, .state = tempPCB.state, .start_time = tempPCB.start_time, .arrival_time = tempPCB.arrival_time, .memsize = tempPCB.memsize, .mem_start_index = tempPCB.mem_start_index, .mem_end_index = tempPCB.mem_end_index});
          
        }
        //////////////////////end of recieving from process_generator//////////////////////
        // 2.run algorithm
        if (Algorithm_num == 0)
        {
            HPF_Algorithm();
        }

        else if (Algorithm_num == 1)
        {
            printf("before  REady queue size:%d\n", ready_Queue_SRTF.size);
            SRTF_Algorithm();
            printf("after algo REady queue size:%d\n", ready_Queue_SRTF.size);
        }

        else if (Algorithm_num == 2)
        {
            RR_Algorithm();
        }
        printf("ProcessTable:\n");
        hashmap_scan(process_table, pcb_iter, NULL);
        if (runingProcess == NULL)
        {
            up(sem3);
            y = semctl(sem3, 0, GETVAL);
            if (y == -1)
            {
                printf("%d", errno);
                printf("Error");
                exit(-1);
            }
            printf("\n***************************** semaphore  in scheduler running process /////////up///// %d *********************\n", y);
        }
        else
            printf("THe ruuning process isn't null so no up\n");
    }
    ///////////////////////////////end main loop//////////////////////////////////////////

    // calculations
    float avgWTA=(float)WTA_sum / Num_Of_Processes;
    fprintf(scheduler_pref, "CPU Utilization = %.2f\n", ((float)all_process / getClk()) * 100);
    fprintf(scheduler_pref, "Avg WTA = %.2f\n", avgWTA);
    fprintf(scheduler_pref, "Avg Waiting = %.2f\n", (float)TA_sum / Num_Of_Processes);

    float sum=0;
    for(int k=0;k<Num_Of_Processes;k++){
        sum+=pow(terminated[k].WTA-avgWTA,2);
    }
    float stdWTA=sqrt(sum/Num_Of_Processes);
    fprintf(scheduler_pref,"Std WTA= %.2f\n",stdWTA);
    fclose(scheduler_pref);
    fclose(scheduler_log);
    fclose(memory_log);

    ////////////////////////////test////////////////////////////////
    printf("\n\nTermiantion Table:\n");
    for (int k = 0; k < j; k++)
    {
        printf("id:%d\n", terminated[k].id);
        printf("arrive:%d\n", terminated[k].arrivaltime);
        printf("crt:%d\n", terminated[k].cumulative_running_time);
        printf("ET :%d\n", terminated[k].excution_time);
        printf("fin:%d\n", terminated[k].finishtime);
        printf("priority:%d\n", terminated[k].priority);
        printf("proces sys:%d\n", terminated[k].process_system_id);
        printf("remain:%d\n", terminated[k].remaining_time);
        printf("start:%d\n", terminated[k].start_time);
        printf("Wt:%d\n", terminated[k].waiting_time);
        printf("TA:%d\n", terminated[k].TA);
        printf("WTA:%f\n\n\n\n", terminated[k].WTA);
    }
    //////////////////////////////////////////////////////////////////

    fflush(stdout);

    // upon termination release the clock resources.
    destroyClk(false);
    exit(1);
}

void HPF_Algorithm()
{
    // if there is no process
    if (!if_process_running)
    {
        // empty ready queue
        if (isEmpty(&ready_Queue_HPF))
            return;
        process_running = peek(&ready_Queue_HPF);
        pop(&ready_Queue_HPF);
        // if (process_running.process_id == -1)
        // {
        //     // empty ready queue
        //     return;
        // }

        struct PCB x = {.process_system_id = process_running.process_system_id};
        runingProcess = hashmap_get(process_table, &x);
        ///////zeinab&&asmaa2
        *shmaddr2 = runingProcess->excution_time;
        print_in_scheduler_log(0, clk, 0, 0, runingProcess,0);
        // send remaining time for first time = run time
        buff_pro_sch buff_pro_sch_mes;
        buff_pro_sch_mes.mtype = runingProcess->process_system_id;
        buff_pro_sch_mes.Remainingtime = runingProcess->excution_time;
        send_val = msgsnd(msgq_pro_sch_id, &buff_pro_sch_mes, sizeof(buff_pro_sch_mes.Remainingtime), !IPC_NOWAIT);
        if (send_val == -1)
            perror("Errror in send");
    }

    if (runingProcess != NULL)
    {
        runingProcess->state = running;
        runingProcess->remaining_time--;
        runingProcess->start_time = clk;
        runingProcess->waiting_time = runingProcess->start_time - runingProcess->arrival_time;
        runingProcess->cumulative_running_time++;
        if_process_running = true;

        printf("\nprocess run %d\n", runingProcess->process_id);
    }
}

void SRTF_Algorithm()
{
    if (one_process_arrived)
    {
        // New process has arrived
        if (runingProcess != NULL)
        {
            // there is running process
            // comapare it with the front
            PCB Front = peek(&ready_Queue_SRTF);
            // compare remaining times
            if (Front.remaining_time < runingProcess->remaining_time)
            {
                // preempt the ruuning ==>stop signal
                // run the Front

                preemptProcess();
                //  int xy = semctl(sem3, 0, GETVAL);
                //  printf("\n***************************** semaphore in process ///////before doen in preempt/////////// %d *********************\n",xy);

                // down(sem3);

                //              xy = semctl(sem3, 0, GETVAL);
                //  printf("\n***************************** semaphore in process ///////after down in preempt/////////// %d *********************\n",xy);
                PCB Front = peek(&ready_Queue_SRTF);
                pop(&ready_Queue_SRTF);
                PCB x = {.process_system_id = Front.process_system_id};
                runingProcess = hashmap_get(process_table, &x);
                run_process();
            }
            else
            {
                // no preemption
                runingProcess->cumulative_running_time++;
                runingProcess->remaining_time--;
            }
        }
        else
        {
            // no process running==> run the front
            // get front Queue
            PCB Front = peek(&ready_Queue_SRTF);
            pop(&ready_Queue_SRTF);
            PCB x = {.process_system_id = Front.process_system_id};
            runingProcess = hashmap_get(process_table, &x);
            run_process();
        }
    }
    else
    {
        // No new process arrived
        if (runingProcess != NULL)
        {
            // there is a process ruuning now
            // 1.inc cumlative running time & dec remaining time
            runingProcess->cumulative_running_time++;
            runingProcess->remaining_time--;
        }
        else if (!isEmpty(&ready_Queue_SRTF))
        {
            printf("Entered here\n");
            // get front Queue
            PCB Front = peek(&ready_Queue_SRTF);
            pop(&ready_Queue_SRTF);
            PCB x = {.process_system_id = Front.process_system_id};
            runingProcess = hashmap_get(process_table, &x);
            run_process();
        }
    }
    one_process_arrived = false;
}

void RR_Algorithm()
{
    //first time to run the quantum
    if (if_quantum_finish == quantum_num)
    {
        process_running = front(&ready_Queue_RR);
        popQueue(&ready_Queue_RR);
        if (process_running.process_id == -1)
        {
            //empty queue
            return;
        }

        struct PCB x = {.process_system_id = process_running.process_system_id};
        runingProcess = hashmap_get(process_table, &x);
        //run for the first time
        if (runingProcess->start_time == -1)
        {
            ///////zeinab&&asmaa2
            *shmaddr2 = runingProcess->excution_time;

            print_in_scheduler_log(0, clk, 0, 0, runingProcess,0);

            //send the remaining time
            runingProcess->acc_running_time_RR = 0;
            buff_pro_sch buff_pro_sch_mes;
            buff_pro_sch_mes.mtype = runingProcess->process_system_id;
            buff_pro_sch_mes.Remainingtime = runingProcess->excution_time;
            send_val = msgsnd(msgq_pro_sch_id, &buff_pro_sch_mes, sizeof(buff_pro_sch_mes.Remainingtime), !IPC_NOWAIT);
            if (send_val == -1)
                perror("Errror in send");
        }
        else
        {
            print_in_scheduler_log(1, clk, 0, 0, runingProcess,0);
            ///////zeinab&&asmaa2
            *shmaddr2 = runingProcess->remaining_time;
            kill(process_running.process_system_id, SIGCONT);
        }
        runingProcess->state = running;
        runingProcess->start_time = clk;
        runingProcess->waiting_time = runingProcess->start_time - runingProcess->arrival_time;
        printf("\nprocess run %d\n", runingProcess->process_id);
    }

    //stop process
    if (if_quantum_finish == 0)
    {
        runingProcess->acc_running_time_RR += quantum_num;
        if (runingProcess->acc_running_time_RR < runingProcess->excution_time)
        {
            printf("\nSTOP\n");
            kill(runingProcess->process_system_id, SIGTSTP);

            print_in_scheduler_log(2, clk, 0, 0, runingProcess,0);

            pushQueue(&ready_Queue_RR, process_running);
            if_quantum_finish = quantum_num;
            //     //============================================================================================//
            process_running = front(&ready_Queue_RR);
            popQueue(&ready_Queue_RR);
            if (process_running.process_id == -1)
            {
                //empty queue
                return;
            }

            struct PCB x = {.process_system_id = process_running.process_system_id};
            runingProcess = hashmap_get(process_table, &x);
            //run for the first time
            if (runingProcess->start_time == -1)
            {
                ///////zeinab&&asmaa2
                *shmaddr2 = runingProcess->excution_time;

                print_in_scheduler_log(0, clk, 0, 0, runingProcess,0);

                //send the remaining time
                runingProcess->acc_running_time_RR = 0;
                buff_pro_sch buff_pro_sch_mes;
                buff_pro_sch_mes.mtype = runingProcess->process_system_id;
                buff_pro_sch_mes.Remainingtime = runingProcess->excution_time;
                send_val = msgsnd(msgq_pro_sch_id, &buff_pro_sch_mes, sizeof(buff_pro_sch_mes.Remainingtime), !IPC_NOWAIT);
                if (send_val == -1)
                    perror("Errror in send");
            }
            else
            {
                print_in_scheduler_log(1, clk, 0, 0, runingProcess,0);
                ///////zeinab&&asmaa2
                *shmaddr2 = runingProcess->remaining_time;
                kill(process_running.process_system_id, SIGCONT);
            }
            runingProcess->state = running;
            runingProcess->start_time = clk;
            runingProcess->waiting_time = runingProcess->start_time - runingProcess->arrival_time;
            printf("\nprocess run %d\n", runingProcess->process_id);
        }
    }
    if_quantum_finish--;
    runingProcess->remaining_time--;
    runingProcess->cumulative_running_time++;
}

void handler_USR1(int sig_num)
{

    int stat_loc;
    pid = wait(&stat_loc);
    printf("in handler %d\n", pid);
    struct PCB x = {.process_system_id = pid};

    struct PCB *pcb;
    pcb = hashmap_delete(process_table, &x);

    //***********************************************************88888======================================DEallocate
    deallocate(pcb->memsize, pcb->mem_start_index, pcb->mem_end_index);
    print_memory_log(clk+1, pcb->memsize, pcb->process_id, pcb->mem_start_index, pcb->mem_end_index, 0);

    //Allocate from the Waiting Queues
    // queue temp = initQueue();
    // while (!isEmpty(&Waiting_Queue_HPF))
    // {

    //     //pop from waiting queue
    //     PCB front_PCB = pop(&Waiting_Queue_HPF);
    //     int end_mem_size;
    //     int start_mem_size = allocate(front_PCB.memsize, &end_mem_size);
    //     if (start_mem_size == -1)
    //     {
    //         //add to temp
    //         pushQueue(&temp, front_PCB);
    //         continue;
    //     }

    //     //move from waiting queue to ready queue
    //     front_PCB.mem_start_index = start_mem_size;
    //     front_PCB.mem_end_index = end_mem_size;

    //     //push in ready queue
    //     if ((Algorithm_num == 0))
    //         push(&ready_Queue_HPF, front_PCB.priority, front_PCB);

    //     else if (Algorithm_num == 1)
    //     {
    //         push(&ready_Queue_SRTF, front_PCB.remaining_time, front_PCB);
    //         one_process_arrived = true;
    //     }
    //     else if (Algorithm_num == 2)
    //     {
    //         pushQueue(&ready_Queue_RR, front_PCB);
    //     }

    //     //update Hash Map
    //     struct PCB x = {.process_system_id = front_PCB.process_system_id};
    //     runingProcess = hashmap_get(process_table, &x);

    //     runingProcess->mem_start_index = front_PCB.mem_start_index;
    //     runingProcess->mem_end_index = front_PCB.mem_end_index;

    //     print_memory_log(clk+1, runingProcess->memsize, runingProcess->process_id, runingProcess->mem_start_index, runingProcess->mem_end_index, 1);
    // }

    // //refill Waiting list
    // while (!isEmptyQueue(&temp))
    // {
    //     PCB front_PCB = front(&temp);
    //     popQueue(&temp);
    //     if (Algorithm_num == 0)
    //     { //HPF
    //         push(&Waiting_Queue_HPF, front_PCB.priority, front_PCB);
    //     }
    //     else if (Algorithm_num == 1)
    //     { //SRTF
    //         push(&Waiting_Queue_HPF, front_PCB.remaining_time, front_PCB);
    //     }
    //     else if (Algorithm_num == 2)
    //     {
    //         push(&Waiting_Queue_HPF, front_PCB.arrival_time, front_PCB);
    //     }
    // }

    if_process_running = false;
    if_quantum_finish = quantum_num;
    int wait_time = clk+1 - pcb->arrival_time - pcb->excution_time + pcb->remaining_time;
    TA_sum += wait_time;
    runingProcess = NULL;

    signal(SIGUSR1, handler_USR1);

    ///////////////calculate data///////////////
    // TA_sum+=(clk+1)-pcb->arrival_time;
    float x1 = (clk + 1) - pcb->arrival_time;
    WTA_sum += x1 / pcb->excution_time;
    all_process += pcb->excution_time;
    standard_avg = 0;
    print_in_scheduler_log(3, clk + 1, (int)x1, x1 / pcb->excution_time, pcb,wait_time);
    ///////////////////////

    ////////////////////////////test////////////////////////

    terminated_process t;
    t.id = pcb->process_id;
    t.arrivaltime = pcb->arrival_time;
    t.cumulative_running_time = pcb->cumulative_running_time;
    t.priority = pcb->priority;
    t.process_system_id = pcb->process_system_id;
    t.remaining_time = pcb->remaining_time;
    t.start_time = pcb->start_time;
    t.finishtime = clk + 1;
    t.excution_time = pcb->excution_time;
    t.waiting_time = pcb->waiting_time;
    t.TA = t.finishtime - t.arrivaltime;
    t.WTA = (float)t.TA / (t.excution_time);
    terminated[j] = t;
    j++;
    //////////////////////////////////////////////////////////////
    up(sem3);
    int y = semctl(sem3, 0, GETVAL);
    if (y == -1)
    {
        printf("%d", errno);
        printf("Error");
        exit(-1);
    }
    printf("\n***************************** semaphore  in scheduler handler /////////up///// %d *********************\n", y);
    tempS=true;
}

void print_in_scheduler_log(int state, int time, int TA, float WTA, PCB *runingProcess,int waitTime)
{
    char *str;
   
    
    if (state == 0)
    {
        str = "Started";
    }
    else if (state == 1)
    {
        str = "Resumed";
    }
    else if (state == 2)
    {
        str = "Stopped";
    }
    else
    {
        str = "Finished";
        fprintf(scheduler_log, "#At\tTime\t%d\tProcess\t%d\t%s\tArrived\t%d\tTotal\t%d\tRemain\t%d\tWait\t%d\tTA\t%d\tWTA\t%.2f\n", time, runingProcess->process_id, str, runingProcess->arrival_time, runingProcess->excution_time, runingProcess->remaining_time, waitTime, TA, WTA);
        return;
    }
     int wait_time = time - runingProcess->arrival_time - runingProcess->excution_time + runingProcess->remaining_time;
    fprintf(scheduler_log, "#At\tTime\t%d\tProcess\t%d\t%s\tArrived\t%d\tTotal\t%d\tRemain\t%d\tWait\t%d\n", time, runingProcess->process_id, str, runingProcess->arrival_time, runingProcess->excution_time, runingProcess->remaining_time, wait_time);
}

void print_memory_log(int clk, int memsize, int process_ID, int start_index, int end_index, int choice)
{
    char *str;
    if (choice == 1)
        str = "allocated";
    else
        str = "freed";
    fprintf(memory_log, "#At\tTime\t%d\t%s\t%d\tbytes\tfor\tprocess\t%d\tfrom\t%d\tto\t%d\n", clk, str, memsize, process_ID, start_index, end_index);
}

void run_process()
{
    if (runingProcess->start_time == -1)
    {
        // send remaining time for first time = run time
        buff_pro_sch buff_pro_sch_mes;
        buff_pro_sch_mes.mtype = runingProcess->process_system_id;
        buff_pro_sch_mes.Remainingtime = runingProcess->excution_time;
        ///////zeinab&&asmaa2
        *shmaddr2 = runingProcess->excution_time;
        send_val = msgsnd(msgq_pro_sch_id, &buff_pro_sch_mes, sizeof(buff_pro_sch_mes.Remainingtime), !IPC_NOWAIT);
        if (send_val == -1)
            perror("Errror in send");
        runingProcess->start_time = clk;
        runingProcess->waiting_time = runingProcess->start_time - runingProcess->arrival_time;
        print_in_scheduler_log(0, clk, 0, 0, runingProcess,0);
    }
    else
    {
        // send to it signal continue ==> was peempted before
        print_in_scheduler_log(1, clk, 0, 0, runingProcess,0);
        ///////zeinab&&asmaa2
        *shmaddr2 = runingProcess->remaining_time;
        kill(runingProcess->process_system_id, SIGCONT);
    }

    runingProcess->state = running;
    runingProcess->remaining_time--;
    runingProcess->cumulative_running_time++;
    return;
}

void preemptProcess()
{
    // 1.send signal stop
    kill(runingProcess->process_system_id, SIGTSTP);

    // 2.change state to waiting
    runingProcess->state = waiting;

    // 3.push it back to the ready queue
    PCB processpreempted = *runingProcess;
    push(&ready_Queue_SRTF, runingProcess->remaining_time, processpreempted);
    print_in_scheduler_log(2, clk, 0, 0, runingProcess,0);
    runingProcess = NULL;
    printf("SIZE OF QUEUE IN PREEMPTION:%d\n", ready_Queue_SRTF.size);

    return;
}
