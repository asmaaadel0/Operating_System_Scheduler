#include "headers.h"
// #include <semaphore.h>
/* Modify this file as needed*/
int remainingtime;
int rec_val;
buff_pro_sch buff_pro_sch_mes;
void handler_Tstop(int signum);
//zeinab&&asmaa2
int Clk;
int *shmaddr2;
int last_update_remaining_time;
int main(int agrc, char *argv[])
{

    signal(SIGTSTP, handler_Tstop);
    printf("pid of process %d\n", getpid());

    //===================================ipcs====================================//
    // Messge queue
    int msgq_pro_sch_id;
    msgq_pro_sch_id = msgget(KEYFILE_pro_sch, 0666 | IPC_CREAT);
    if (msgq_pro_sch_id == -1)
    {
        perror("Error in msgget()");
        exit(-1);
    }
    //recieve initial remaining time
    rec_val = msgrcv(msgq_pro_sch_id, &buff_pro_sch_mes, sizeof(buff_pro_sch_mes.Remainingtime), getpid(), !IPC_NOWAIT);
    if (rec_val == -1)
        perror("Error in receive");

    // TODO it needs to get the remaining time from somewhere
    remainingtime = buff_pro_sch_mes.Remainingtime;
    printf("process %d recieved remaining time %d\n", getpid(), remainingtime);
    //zeinab&&asmaa
     int shmid2;
    shmid2 =shmget(SHKEY2, 256, IPC_CREAT | 0644);
   if (shmid2 == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    shmaddr2 = (int *)shmat(shmid2, (void *)0, 0);
//=========================================================================================//
    //initialize clk
    initClk();
    Clk = getClk();
    last_update_remaining_time=remainingtime;
    int sem3 = semget(SEM3KEY, 1, 0666 | IPC_CREAT);
    ///////////////////////////////////////main loop////////////////////////////////////
     while (remainingtime > 0)    
    {
        
       signal(SIGTSTP, handler_Tstop);
        while (Clk == getClk());///////////////up done
       
    printf("\nhiiiiiiiiiiiiiiiiiiiiiii%d  %d\n",getClk(),Clk);
        if( getClk() - Clk > 1){
            Clk = getClk();
              printf("\nenter last_update_if%d\n",*shmaddr2);
            last_update_remaining_time=*shmaddr2;
            remainingtime=last_update_remaining_time;
            // down(sem3);
            continue;
        }
        Clk = getClk();
         remainingtime--;
       printf("clk in process: %d  remaining time:%d  pid%d\n", Clk, remainingtime,getpid());
        // remainingtime--;
          int x = semctl(sem3, 0, GETVAL);
        if (x == -1)
        {
        printf("%d", errno);
        printf("Error");
        exit(-1);
        }
        printf("\n***************************** semaphore in process %d///////before up/////////// %d *********************\n",getpid(),x);
        if(remainingtime!=0)
           up(sem3);
         x = semctl(sem3, 0, GETVAL);
        if (x == -1)
        {
        printf("%d", errno);
        printf("Error");
        exit(-1);
        }
        printf("\n***************************** semaphore in process %d///////up/////////// %d *********************\n",getpid(),x);
        Clk = getClk();
    }

    //fflush(stdout);
    //notify schedular 
    // down(sem3);
    kill(getppid(), SIGUSR1);
    printf("clk in process 2:******************* %d *****************",getClk());
    destroyClk(false);

    return 0;
}

void handler_Tstop(int signum)
{
    Clk--;
    signal(SIGTSTP,SIG_DFL);
    kill(getpid(),SIGTSTP);
}