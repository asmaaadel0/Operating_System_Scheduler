#include "headers.h"
//#include"Queue.c"
void clearResources(int);

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
    }
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
    // 4. Use this function after creating the clock process to initialize clock
   
    initClk();
    // To get time use this
//printf("%d\n",x);
  j=0;
  int ck;

 while(j<size)
{
    ck = getClk();
 while(j<size&&dataStruct[j].arrive<=ck)
 {
    printf("%d\n",dataStruct[j].id);
    printf("%d\n",ck);
     j++;
 }

 }


    printf("current time is %d\n", ck);
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