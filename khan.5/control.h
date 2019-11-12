
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
#include <ctype.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ipc.h>	//Inter process commication

#define ADDRESS 900
#define Max_Process 18	// Max number of processes allowed
#define SHM_KEY 786786	//Key for shared memory pointer
#define MESSAGEKEY 3000
#define TotalRes 20	//Total number of processes

// Clock implementation
typedef struct system_clock {
	int seconds;
	int milliseconds;
	int nanoSeconds;
} Clock;

//message queue implementation
typedef struct message {
	long messageAddress;
	int returnAddress;
} Message;

//Process control Block implementation
typedef struct ResourceDiscriptor {
	int allocated[TotalRes];
	int requested[TotalRes];
	int Terminated;
	int Pid;
	int deadlockRes[TotalRes];
} resDis;

typedef struct Resources {
	int max[]; // Hpw many total instances of this resources
	int available[]; //How many available instances of this resources
}resources;
//to share the process control block for 18 process.
typedef struct SharedMemoryObject{
resource resources;
Clock clockInfo;
resDis ResourceDis[Max_Process];
} SharedMemory;


