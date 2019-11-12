//
// Created by crbaniak on 11/10/18.
//

#include "header.h"

int main(int argc, char* argv[]){

    // ##### shared memory config #####
    sharedMemoryConfig();

    // ##### msgQueue config #####
    messageQueueConfig();

    // ###### variables #####
    int timeIncrement, rollover;
    int seconds = sysClockshmPtr->seconds;
    int nanoseconds = sysClockshmPtr->nanoseconds;


    // get argv values for randClockTime sent in
    timeIncrement = atoi(argv[0]);

    // adjust request timer
    if ((nanoseconds + timeIncrement) > 999999999){
        rollover = (nanoseconds + timeIncrement) - 999999999;
        seconds += 1;
        nanoseconds = rollover;
    } else {
        nanoseconds += timeIncrement;
    }

//    printf("We in this. %d     %d\n", getpid(), timeIncrement);

    while(requestTimeReached == 0){

        if(sysClockshmPtr->seconds >= seconds && sysClockshmPtr->nanoseconds >= nanoseconds){

            sprintf(message.mesg_text, "%d", getpid());

//            strcpy(message.mesg_text, "A message from the msgQ");
            message.mesg_type = 1;

            // msgsnd to send message queue
            msgsnd(msgid, &message, sizeof(message), 0);

            requestTimeReached = 1;
        }

    }

//    printf("We out this. %d\n", getpid());

    // clean shared mem
    shmdt(sysClockshmPtr);
    shmdt(RDPtr);

    exit(0);
}

