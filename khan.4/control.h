
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
#define TIME_QUANTUM 20000
#define LINE_MAX 10000
#define MaxUserProcess 18
#define CONSTANTNUMBER 500
#define SHM_KEY 786786
#define MESSAGEKEY 3000
#define SetBit(A, k)	A[k/32] |= 1 << (k%32); // To Set the k'th bit in the array A.
#define ClearBit(A, k)	A[k/32]	&= ~(1 << (k%32)); //To clear the k'th bit in the bit array A.
#define TestBit(A,k)	(A[((k-1)/32)]&(1<<((k-1)%32))) // To test the k'th bit in the bit array A.

typedef struct Node_Object NodeObject;
typedef struct Node_Object {
	int process_id;
	NodeObject* next_node;
} NodeObject;

typedef struct Queue_Object {
    NodeObject* front;
    NodeObject* back;
    int time_quantum;
} queueObject;

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


// Function to create Queues for multi level queues
queueObject* createQueue(int time_quantum) {
        queueObject* new_queue = (queueObject*) malloc(sizeof(queueObject));

        new_queue->time_quantum = time_quantum;
                              new_queue->front = NULL;
                              new_queue->back = NULL;
                              return new_queue;
}
// Funtion to push the process ID's to the queue. 
int push(queueObject* targetQueue, int processID) {

    if (targetQueue == NULL) {
        return -1;
    }

    NodeObject* new_queue_node = (NodeObject*) malloc(sizeof(NodeObject));
    new_queue_node->process_id = processID;
    new_queue_node->next_node = NULL;


    if (targetQueue->front == NULL) {
        targetQueue->front = new_queue_node;
        targetQueue->back = new_queue_node;
    }

    else {
        targetQueue->back->next_node = new_queue_node;
        targetQueue->back = new_queue_node;
    }


    return 0;
}


//Function to delete the node 
int pop(queueObject* sourceQueue) {


        if(sourceQueue->front == NULL){
               return -1; //this is an empty queue
         }

        int processID = sourceQueue->front->process_id;


        NodeObject* temp_node = sourceQueue->front;
        sourceQueue->front = sourceQueue->front->next_node;
        free(temp_node);

        return processID;
 }



