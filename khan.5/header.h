//
// Created by crbaniak on 11/10/18.
//

#ifndef ASS5_HEADER_H
#define ASS5_HEADER_H

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

//shared mem keys, probs not secure in a header, but its here to stay
#define CLOCK_SHMKEY 123123
#define RD_SHMKEY 123124

// ##### SHMEM STRUCTS #####
// struct for time
typedef struct {
    unsigned int seconds;
    unsigned int nanoseconds;
} systemClock_t;

//struct for rescource descriptor
typedef struct {
    // for managing pids, job of process, and time request iintervals
    pid_t pids[18];
    int pidJob[18];
    int nanosRequest[18];

    // tables for safety alg (banker's alg)
    int rescources[20];
    int max[18][20];
    int allocated[18][20];
    int request[18][20];

} rescourceDescriptor_t;

// struct for message queue
struct mesg_buffer {
    long mesg_type;
    char mesg_text[100];
} message;


// ##### GLOBALS #####
// globals for accessing pointers to shared memory
int sysClockshmid; //holds the shared memory segment id
systemClock_t *sysClockshmPtr; //points to the data structure
int RDshmid;
rescourceDescriptor_t *RDPtr;
int totalLines = 0; // total lines in log file
int pidHolder[18] = {};
int randomClockTime[18] = {};
int blockedQueue[18] = {};
int requestTimeReached = 0;
//msg q
key_t key;
int msgid;


// allocates shared mem
void sharedMemoryConfig() {

    //shared mem for sysClock
    sysClockshmid = shmget(CLOCK_SHMKEY, sizeof(systemClock_t), IPC_CREAT|0777);
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

void messageQueueConfig(){
    // ftok to generate unique key
    key = ftok("progfile", 65);
    // msgget creates a message queue
    // and returns identifier
    msgid = msgget(key, 0666 | IPC_CREAT);
}

#endif //ASS5_HEADER_H
