#ifndef HEADER_H
#define HEADER_H
#include <stdio.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <string.h>
#include <errno.h>//for error no

typedef short bool;
#define true 1
#define false 0

/////////////////////////////////////ipcs////////////////////////////////
#define KEYFILE 60 // for message queue between process_gerenarator and schedular
#define KEYFILE_pro_sch 80 // for message queue between process and schedular

#define SHKEY 300 //shared memory
#define SHKEY2 400
#define SEM1KEY 1 // for sem1
#define SEM2KEY 2 // for sem2
#define SEM3KEY 3 // for sem3 between process and scheduler
/////////////////////////////end ipcs////////////////////////////////////

///==============================
// don't mess with this variable//
int *shmaddr; //
//===============================
/////////////////////////////////////struct///////////////////////////////////////

//for status of the process
enum status
{
    waiting,
    running
};

typedef struct PCB
{
    int process_id;
    int arrival_time;
    int excution_time;
    int priority;
    enum status state;
    int waiting_time;
    int remaining_time;
    int cumulative_running_time;
    int process_system_id;
    int start_time;
    int acc_running_time_RR ;

	int memsize;
    int mem_start_index;
    int mem_end_index;
} PCB;

typedef struct terminated_process
{
    int id;
    int priority;
    int arrivaltime;
    int start_time;
    int excution_time;
    int waiting_time;
    int remaining_time;
    int cumulative_running_time;
    int process_system_id;
    int finishtime;
    int TA;  // finsh - arrive
    float WTA; // TA/run

} terminated_process;
//////////////////////////////////message buffer//////////////////////////////////
typedef struct Algorithmmsgbuff
{
    long mtype;
    int Algorithm;
    int quantum;
} Algorithmmsgbuff;

typedef struct buff_pro_sch
{
    long mtype;
    int Remainingtime;
} buff_pro_sch;

typedef struct processmsgbuff
{
    long mtype;
    PCB process;
    int Noofprocesses_to_Send;
    bool End;

} processmsgbuff;
////////////////////////////////////clk functions/////////////////////////////////////

int getClk()
{
    return *shmaddr;
}

/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
 */
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        // Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}

/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
 */

/////////////////////////////////////semaphore functions////////////////////////////////

void down(int sem)
{
    struct sembuf d_op;
    d_op.sem_num = 0;
    d_op.sem_op = -1;
    d_op.sem_flg = !IPC_NOWAIT;
    int val;
    while ((val = semop(sem, &d_op, 1)) == -1 && errno == EINTR)
        continue;
    if (val == -1)
    {
        perror("Error in down()\n");
        exit(-1);
    }
}

void up(int sem)
{
    struct sembuf d_op;
    d_op.sem_num = 0;
    d_op.sem_op = 1;
    d_op.sem_flg = !IPC_NOWAIT;
    int val;
    while ((val = semop(sem, &d_op, 1)) == -1 && errno == EINTR)
        continue;
    if (val == -1)
    {
        perror("Error in up()\n");
        exit(-1);
    }
}

union Semun
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /*buffer for IPC_STAT & IPC_SET */
    ushort *array;         /*array for GETALL & SETALL */
    struct seminfo *__buf; /*buffer for IPC_INFO */
    void *__pad;
};

union Semun semun;

int getsem(int key)
{

    int sem = semget(key, 1, 0666 | IPC_CREAT);
    if (sem == -1)
    {
        perror("Error in create sem");
        exit(-1);
    }
    semun.val = 0;
    if (semctl(sem, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }
    return sem;
}

#endif