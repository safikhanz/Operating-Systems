#include <stdio.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>

//shared mem keys
#define CLOCK_SHMKEY 786786
#define RD_SHMKEY 786781
#define max_proc 18
#define max_resources 20
#define MessQ_Key 786782

// ##### SHMEM STRUCTS #####
// struct for time
typedef struct {
    unsigned int seconds;
    unsigned int nanoseconds;
} Clock_t;

//struct for rescource descriptor
typedef struct {
    // for managing pids, job of process, and time request iintervals
    pid_t pids[max_proc];
    int pidJob[max_proc];
    int nanosRequest[max_proc];

    // tables for safety alg (banker's alg)
    int rescources[max_resources];
    int max[max_proc][max_resources];
    int allocated[max_proc][max_resources];
    int request[max_proc][max_resources];

} rescourceDescriptor_t;

// struct for message queue
struct mesg_buffer {
    long mesg_type;
    char mesg_text[100];
} message;


// ##### GLOBALS #####
// globals for accessing pointers to shared memory
int sysClockshmid; //holds the shared memory segment id
Clock_t *sysClockshmPtr; //points to the data structure
int RDshmid;
rescourceDescriptor_t *RDPtr;
int totalLines = 0; // total lines in log file
int pidHolder[18] = {};
int randomClockTime[18] = {};
int blockedQueue[18] = {};
int requestTimeReached = 0;
//msg q
int msgid;

void messageQueueConfig(){
    
    // msgget creates a message queue
    // and returns identifier
    msgid = msgget(MessQ_key, 0666 | IPC_CREAT);
}

// allocates shared mem
void sharedMemoryConfig() {

    //shared mem for sysClock
    sysClockshmid = shmget(CLOCK_SHMKEY, sizeof(Clock_t), IPC_CREAT|0777);
    if(sysClockshmid < 0)
    {
        perror("sysClock shmget error in master \n");
        exit(errno);
    }
    sysClockshmPtr = shmat(sysClockshmid, NULL, 0);
    if(sysClockshmPtr < 0){
        perror("sysClock shmat error in oss\n");
        exit(errno);
    }

    //shared mem for Rescource Descriptor
    RDshmid = shmget(RD_SHMKEY, sizeof(rescourceDescriptor_t), IPC_CREAT|0777);
    if(RDshmid < 0)
    {
        perror("RD shmget error in master \n");
        exit(errno);
    }
    RDPtr = shmat(RDshmid, NULL, 0);
    if(RDPtr < 0){
        perror("sysClock shmat error in oss\n");
        exit(errno);
    }

}



