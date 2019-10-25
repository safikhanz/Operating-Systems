#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/shm.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>	
#include <semaphore.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <signal.h>

#define SHMKEY 786786



int main () {

	sem_t *sem;
	int exampleSize[3];

	int randomNum = (rand() % (1000000-1)) + 1;

	int shmid = shmget ( SHMKEY, sizeof(exampleSize[3]), 0775 | IPC_CREAT );
char * paddr = ( char * )( shmat ( shmid, NULL, 0 ) );
  int * cint = ( int * )( paddr );


 	int nanos = cint[2];
	int seconds = cint[1];
	int rollover;

    if ((nanos + randomNum) > 999999999){
        rollover = (nanos + randomNum) - 999999999;
        seconds += 1;
        nanos =0;
    }

    if(nanos == cint[2] && seconds == cint[1]){
        cint[3] = getpid();
        shmdt(cint);
        return 0;
    }


    shmdt(cint);
return 0; 
}
