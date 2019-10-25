#include "control.h"


char outputFileName[] = "log.txt";
static int timer = 2;
static int msgid;
static pid_t actual[MaxUserProcess + 1];
Clock maxTimeBetweenProc;

SharedMemory* shm_ptr;
FILE* fp;

int maxProcess = 100;
int line = 1;
int terminateProcessCount = 0;
int queueLevel = 4;
int processTable[32];
int ProcPid;
int shmid;

double avgTAT = 0.00;
unsigned long long totalTAT = 0;
double avgWaitTime = 0.00;
unsigned int cpuIdleTime = 0;

void helpMenu();
int selection(void);
void init_PCB(int location);
void userOrReal(int ProcPid);
void getInterval(Clock *destinationClock, Clock sourceClock, int addNanoSeconds);
int getPCBID(int array[]);
void signalCall(int signum);
void mailMessage(int destinationAddress);
void recieveMessage(int destinationAddress);
void makeUserProcesses(int location);
void createQueueSystem();


int main(int argc, char* argv[]) {
        int c;

        while((c = getopt (argc,argv, "hi:")) != -1) {

                switch(c) {
                        case 'h':
                                helpMenu();
                                return 1;
                        case 'i':
                                strcpy(outputFileName, optarg);
                                break;
                        default:
                                fprintf(stderr, "%s: Error: Unknown option -%c\n",argv[0],optopt);
                                return -1;
                }


        }


    if (signal(SIGALRM, signalCall) == SIG_ERR) {
        perror("Error: child: signal(): SIGALRM\n");
        exit(errno);
                                                                       
}

   alarm(timer);

//Shared memory
   
    if ((shmid = shmget(SHM_KEY, sizeof(SharedMemory), IPC_CREAT | 0600)) < 0) {
        perror("Error: shmget");
        exit(errno);
    }
//Shared message queue
         
    if ((msgid = msgget(MESSAGEKEY, IPC_CREAT | 0600)) < 0) {
        perror("Error: mssget");
        exit(errno);
    }

//attaching the shared memory and initializing the clock to 0
    shm_ptr = shmat(shmid, NULL, 0);
    shm_ptr->clockInfo.seconds = 0;
    shm_ptr->clockInfo.nanoSeconds = 0;
    maxTimeBetweenProc.seconds = 0;
    maxTimeBetweenProc.nanoSeconds = 0;

    int pidSelection;// This is for process selection by their ID's

    createQueueSystem();

    fp = fopen(outputFileName, "w"); 

   int totalCount =0; 
    while(totalCount < maxProcess) {

//getinterval from the start random
        getInterval(&shm_ptr->clockInfo, shm_ptr->clockInfo, rand() % CONSTANTNUMBER + 1);
//If the line in the file contains more than 1000 then close the file.
        if(line > LINE_MAX) {
            fclose(fp);
        }


//Checking the launch time to launch user processes at random interval
        if ((shm_ptr->clockInfo.seconds > maxTimeBetweenProc.seconds) ||
        (shm_ptr->clockInfo.seconds == maxTimeBetweenProc.seconds && shm_ptr->clockInfo.nanoSeconds > maxTimeBetweenProc.nanoSeconds)) {


                ProcPid = getPCBID(processTable);


            if (ProcPid != -1) {
                makeUserProcesses(ProcPid);
                        totalCount++;
                ++line;


                SetBit(processTable, ProcPid);
push(multilevelQueue[shm_ptr->ProcessControlBlock[ProcPid].priority], ProcPid);
		//INCREASE THE CLOCK FOR NEW PROCSS TO GENERATE
                   getInterval(&maxTimeBetweenProc, shm_ptr->clockInfo, rand() % CONSTANTNUMBER + 1);

                   fprintf(fp, "OSS: Generating process with PID %d  at time %d:%d\n",  ProcPid, shm_ptr->clockInfo.seconds, shm_ptr->clockInfo.nanoSeconds);
                    ++line;
            }
        }

//Selecting the queue
        pidSelection = selection();


        if(pidSelection != -1){


            mailMessage(pidSelection);
            fprintf(fp, "OSS: Dispatching process with PID %d  into queue %d at time %d:%d \n", pidSelection, shm_ptr->ProcessControlBlock[ProcPid].priority, shm_ptr->clockInfo.seconds, shm_ptr->clockInfo.nanoSeconds);
            ++line;


            recieveMessage(ADDRESS);
                shm_ptr->ProcessControlBlock[ProcPid].process_arrives.seconds = shm_ptr->clockInfo.seconds;
                shm_ptr->ProcessControlBlock[ProcPid].process_arrives.nanoSeconds = shm_ptr->clockInfo.nanoSeconds;
            }
        }



    avgWaitTime = cpuIdleTime/terminateProcessCount;
    fprintf(stderr,"Average Wait Time = %.0f nanoSeconds per process\n", avgWaitTime);
    avgTAT = totalTAT/terminateProcessCount;
    fprintf(stderr,"Average Turn Around Time = %.0f nanoSeconds per process\n", avgTAT);
    fprintf(stderr,"CPU Idle Time = %u nanoSeconds\n", cpuIdleTime);
    msgctl(msgid, IPC_RMID, NULL);
    fprintf(stderr,"\nline count = %d\n", line);
     shmdt(shm_ptr); //detaches a section of shared memory
    shmctl(shmid, IPC_RMID, NULL);  // deallocate the memory
   fclose(fp);

    return 0;
}


//Initializing the process control block to record stats
void init_PCB(int location){
        shm_ptr->ProcessControlBlock[location].priority = 0;
        shm_ptr->ProcessControlBlock[location].cpu_usage_time = 0;
        shm_ptr->ProcessControlBlock[location].burst = 0;
        shm_ptr->ProcessControlBlock[location].time_quantum = multilevelQueue[shm_ptr->ProcessControlBlock[location].priority]->time_quantum;
        shm_ptr->ProcessControlBlock[location].finished = 0;
        shm_ptr->ProcessControlBlock[location].process_starts.seconds = shm_ptr->clockInfo.seconds;
        shm_ptr->ProcessControlBlock[location].process_starts.nanoSeconds = shm_ptr->clockInfo.nanoSeconds;
        shm_ptr->ProcessControlBlock[location].process_arrives.seconds = 0;
        shm_ptr->ProcessControlBlock[location].process_arrives.nanoSeconds = 0;
}



int selection(void) {
    int i;
    int selectedProcess;


    for(i = 0; i < queueLevel; ++i) {
        selectedProcess = pop(multilevelQueue[i]);

         if (selectedProcess == -1) {
            continue;
        }

        return selectedProcess;
    }


    return -1;
}

// function to select the priority of the processes
void userOrReal(int ProcPid) {

    int currentPriority = shm_ptr->ProcessControlBlock[ProcPid].priority;


    if (shm_ptr->ProcessControlBlock[ProcPid].burst == multilevelQueue[shm_ptr->ProcessControlBlock[ProcPid].priority]->time_quantum) {

        shm_ptr->ProcessControlBlock[ProcPid].priority = (currentPriority + 1 >= 3) ? currentPriority : ++currentPriority;
        shm_ptr->ProcessControlBlock[ProcPid].time_quantum = multilevelQueue[currentPriority]->time_quantum;
    }


    else {

        fprintf(fp, "OSS: PID %d not using entire time quantum\n", ProcPid);
        ++line;


        currentPriority = 0;
        shm_ptr->ProcessControlBlock[ProcPid].priority = currentPriority;
        shm_ptr->ProcessControlBlock[ProcPid].time_quantum = multilevelQueue[currentPriority]->time_quantum;
    }


    push(multilevelQueue[currentPriority], ProcPid);


    fprintf(fp, "OSS: Putting process with PID %d into queue %d\n",ProcPid, currentPriority);
    ++line;
}

void getInterval(Clock *destinationClock, Clock sourceClock, int addNanoSeconds) {
    destinationClock->nanoSeconds = sourceClock.nanoSeconds + addNanoSeconds;
    if(destinationClock->nanoSeconds > 1000000000) {
        destinationClock->seconds++;
        destinationClock->nanoSeconds -= 1000000000;
    }

}




void signalCall(int signum)
{
    int status;

    if (signum == SIGINT)
        printf("\nSIGINT received by main\n");
    else
        printf("\nSIGALRM received by main\n");

    while(wait(&status) > 0) {
        if (WIFEXITED(status))
                printf("User process exited with value %d\n", WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
                printf("User process exited due to signal %d\n", WTERMSIG(status));
        else if (WIFSTOPPED(status))
                printf("User process was stopped by signal %d\n", WIFSTOPPED(status));
    }



    avgWaitTime = cpuIdleTime/terminateProcessCount;
    fprintf(stderr,"Average Wait Time = %.0f nanoSeconds per process\n", avgWaitTime);
    avgTAT = totalTAT/terminateProcessCount;
    fprintf(stderr,"Average Turn Around Time = %.0f nanoSeconds per process\n", avgTAT);
    fprintf(stderr,"CPU Idle Time = %u nanoSeconds\n", cpuIdleTime);
    msgctl(msgid, IPC_RMID, NULL);
    fprintf(stderr,"\nline count = %d\n", line);

   kill(0, SIGTERM);

    shmdt(shm_ptr); //detaches a section of shared memory
    shmctl(shmid, IPC_RMID, NULL);
   fclose(fp);

      exit(EXIT_SUCCESS);
 }


void mailMessage(int destinationAddress){
    static int size_of_message;
    Message message;
    message.messageAddress = destinationAddress;
size_of_message = sizeof(message) - sizeof(message.messageAddress);
    msgsnd(msgid, &message, size_of_message, 0);
}


void recieveMessage(int destinationAddress){
    static int size_of_message;
    Message message;
    size_of_message = sizeof(message) - sizeof(long);
    msgrcv(msgid, &message, size_of_message, destinationAddress, 0);

    int fakePid = message.returnAddress;


    fprintf(fp, "OSS: Receiving that process with PID %d ran for %d nanoseconds\n", ProcPid,shm_ptr->clockInfo.nanoSeconds);
    ++line;

    int status;
    getInterval(&shm_ptr->clockInfo, shm_ptr->clockInfo, shm_ptr->ProcessControlBlock[ProcPid].burst);



    if(shm_ptr->ProcessControlBlock[ProcPid].finished == 1) {
        shm_ptr->ProcessControlBlock[ProcPid].wait_time = ((shm_ptr->clockInfo.seconds + shm_ptr->clockInfo.nanoSeconds) - (shm_ptr->ProcessControlBlock[ProcPid].process_starts.seconds + shm_ptr->ProcessControlBlock[ProcPid].process_starts.nanoSeconds) - shm_ptr->ProcessControlBlock[ProcPid].cpu_usage_time);
      cpuIdleTime += shm_ptr->ProcessControlBlock[ProcPid].wait_time;

        shm_ptr->ProcessControlBlock[ProcPid].turn_around_time = (((shm_ptr->clockInfo.seconds * 1000000000) + shm_ptr->clockInfo.nanoSeconds) - ((shm_ptr->ProcessControlBlock[ProcPid].process_arrives.seconds * 1000000000) + shm_ptr->ProcessControlBlock[ProcPid].process_arrives.nanoSeconds));
        fprintf(fp, "OSS: PID %d turnaround time = %d nanoSeconds\n",ProcPid, shm_ptr->ProcessControlBlock[ProcPid].turn_around_time);
        ++line;


        totalTAT += shm_ptr->ProcessControlBlock[ProcPid].turn_around_time;

        kill(actual[ProcPid], SIGINT);
        ++terminateProcessCount;

        waitpid(actual[ProcPid], &status, 0);

        ClearBit(processTable, ProcPid);


        fprintf(fp, "OSS: PID %d terminated at time %d:%d\n", ProcPid, shm_ptr->clockInfo.seconds, shm_ptr->clockInfo.nanoSeconds);
        ++line;
        fprintf(fp, "OSS: PID %d burst time %d\n", ProcPid, shm_ptr->ProcessControlBlock[ProcPid].burst);
        ++line;
    }


    else {
        userOrReal(ProcPid);

    }
}
// To get the index of the process
int getPCBID(int array[]) {
   int i;
    for (i = 1; i <= makeUserProcesses; i++) {
        if(!TestBit(array, i)) {
            return i;
        }
        return -1;

    }
}



void makeUserProcesses(int location) {
        char childId[3];


        init_PCB(location);
        pid_t childpid = fork();
        actual[location] = childpid;

        if (childpid < 0) {

            perror("Fork failed");
            exit(errno);
        }

        else if (childpid == 0) {
            sprintf(childId, "%d", location);
            execl("./user", "user", childId, NULL);
            perror("Error");

        }

    }

void createQueueSystem(){


    int k=0;
    for (k = 0; k < 4; k++){
        multilevelQueue[k] = createQueue(TIME_QUANTUM * 1);
    }

    int j = 0;
    for (j = 0; j <= 32; j++) {
        processTable[j] = 0;
    }



}

void helpMenu() {
		printf("\n------------| Help Menu |---------------\n");
		printf("-h help menu\n"); 
		printf("-i filename	| filename of the log file.\n"); 
		printf("-------------------------------------------\n");
}

