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

#define SHMKEY 123123



int main () {



    int exampleSize[3];


    int randomNum = (rand() % (1000000-1)) + 1;

    int shmid = shmget ( SHMKEY, sizeof(exampleSize[3]), 0775 | IPC_CREAT );

    int *cint = ( shmat ( shmid, NULL, 0 ) );


    int nanos = cint[1];
    int seconds = cint[0];
    int rollover = 0;


    if ((nanos + randomNum) > 999999999){
        rollover = (nanos + randomNum) - 999999999;
        seconds += 1;
        nanos += rollover;
    }




    if(nanos == cint[1] && seconds == cint[0]){
        cint[2] = getpid();
        shmdt(cint);
        return 0;
    }


    shmdt(cint);
return 0; 
}
