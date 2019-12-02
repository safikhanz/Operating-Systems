
#ifndef ASS6_HEADER_H
#define ASS6_HEADER_H

#include <stdio.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>


#define MAX_PROCS 18
#define SHMKEY 123123

// #####
// ##### STRUCTURES #####
// #####

// struct for time
typedef struct {
    unsigned int seconds;
    unsigned int milliseconds;
} systemClock_t;

// frame table
typedef struct {
    int frames[256];
    int PIDS[256];
    int referenceFlag[256];
    int dirtyBit[256];
    int occupied[256];
} frameTable_t;

// page table
typedef struct {
    int pages[32];
} pageTable_t;

//for shared memory
typedef struct {
    int sharedPIDHolder[18];         // for checking program termination
    int checkProcNum[18];
    int processAddressCalled[18];
    int processReadOrWrite[18];
    int processCallCount[18];
} shared_t;

struct mesg_buffer {
    long mesg_type;
    char mesg_text[100];
} message;

// #####
// ##### VARIABLES #####
// #####

frameTable_t frameTable;        // define frame table
pageTable_t pageTable[MAX_PROCS];   // define array of page tables
systemClock_t theClock;         // define clock
int sharedShmid;                // shmem - holds the shared memory segment id
shared_t *sharedShmptr;            // shmem - points to the data structure

int msgid;                      // for message queue
key_t key;                      // for message queue

int proccessesRunning = 0;      // variable that terminates program

int PIDHolder[18] = {};    //the main pid holder user to check for termination
int numForksMade = 0;
int randTimeToFork[18];         // each processes random time to fork

int procsRunning = 0;

int numWritesToLog = 0;

int PID;        // holds a temp PID


void sharedMemoryConfig() {
    //shared mem for sysClock

    sharedShmid = shmget(SHMKEY, sizeof(systemClock_t), IPC_CREAT|0777);
    if(sharedShmid < 0)
    {
        perror("sysClock shmget error in master \n");
        exit(errno);
    }
    sharedShmptr = shmat(sharedShmid, NULL, 0);
    if(sharedShmptr < 0){
        perror("sysClock shmat error in oss\n");
        exit(errno);
    }
}

void messageQueueConfig(){
        // ftok to generate unique key
        key = ftok("progfile", 65);
        // msgget creates a message queue
        // and returns identifier
        msgid = msgget(key, 0666 | IPC_CREAT);
}

void initRandomForkTimes(){

    // init clock to random
    time_t t;
    srand((unsigned) time(&t));

    int ii;
    for(ii = 0; ii < 18; ii++){
        randTimeToFork[ii] = (rand() % (500 - 100)) + 100;
    }

}

#endif //ASS6_HEADER_H




