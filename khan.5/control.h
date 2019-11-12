
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/shm.h>
#include <string.h>
#include <math.h>

#define ADDRESS 900
#define MaxUserProcess 18
#define SHM_KEY 786786
#define MESSAGEKEY 3000

// Clock implementation
typedef struct system_clock {
    int seconds;
    int nanoSeconds;
} Clock;

//message queue implementation
typedef struct message {
   long messageAddress;
        int returnAddress;
} Message;

//Process control Block implementation
typedef struct processCB {
Clock process_starts; //To store the time of the process
Clock process_arrives;// To store the time of the arrival of the process 
int turn_around_time;
int wait_time;
int priority, burst, time_quantum, finished, cpu_usage_time;
} PCB;

//to share the process control block for 18 process.
typedef struct SharedMemoryObject{
PCB ProcessControlBlock[MaxUserProcess];
Clock clockInfo;
} SharedMemory;

typedef struct ResourceDiscriptor{
	int allocated[];
	int request[];

}
