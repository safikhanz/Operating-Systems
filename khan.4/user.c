#include "control.h"

int childId;
int shmid;
SharedMemory* shm_ptr;
static int msgId;


void mailMessage(int destination_address, char* termMessage);
void recieveMessage(int destination_address);

int main(int argc, char* argv[])
{
	
	childId = atoi(argv[1]);


//intitialzing the shared memory
    if ((shmid = shmget(SHM_KEY, sizeof(SharedMemory), 0600)) < 0) {
        perror("Error: shmget");
        exit(errno);
    }
  
    //initializing the message  queue 
    if ((msgId = msgget(MESSAGEKEY, 0600)) < 0) {
        perror("Error: msgget");
        exit(errno);
    }

   //attaching the shared memory pointer
    shm_ptr = shmat(shmid, NULL, 0);
   //for time
    srand(time(NULL));
//Time given to the process to execute
    int timeExecution = rand() % 40000;

    char *termMessage;

    while(1){

	
        recieveMessage(childId);
        
	// calculating the terminating probability.
        int termProb = rand() % 100;
   
        if (termProb == 0) {
            shm_ptr->processCB[childId].burst = shm_ptr->processCB[childId].time_quantum;
        } else {
            shm_ptr->processCB[childId].burst = rand() % shm_ptr->processCB[childId].time_quantum;
        }

	
        int execRemaining = timeExecution - shm_ptr->processCB[childId].cpu_usage_time;
        if (shm_ptr->processCB[childId].burst > execRemaining) {
            shm_ptr->processCB[childId].burst = execRemaining;
            termMessage = "blocked";
        }

        shm_ptr->processCB[childId].cpu_usage_time += shm_ptr->processCB[childId].burst;
        
	//if the process is completed, send message to the oss 
        if (shm_ptr->processCB[childId].cpu_usage_time >= timeExecution) {
            shm_ptr->processCB[childId].finished = 1;
            termMessage = "terminated";
        }

        mailMessage(ADDRESS, termMessage);
    }

    return 0;
}


void mailMessage(int address,char *termMessage) {
    static int sizeofMessage;
    Message message;
    message.messageAddress = address;
    message.returnAddress = childId;
    sizeofMessage = sizeof(message) - sizeof(long);
    msgsnd(msgId, &message, sizeofMessage, 0);
}


void recieveMessage(int address) {
    static int sizeofMessage;
    Message message;
    sizeofMessage = sizeof(message) - sizeof(long);
    msgrcv(msgId, &message, sizeofMessage, address, 0);
}
