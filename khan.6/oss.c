#include "header.h"

void sigint(int);
void checkMsgQ();
void cleanup();
void createProcess();
void runCountCheckForTermination();
void memoryManagement();
void writeLog(int, int, int);
void checkPageTable(int);
void searchFTable(int, int);
void initDirtyBits();
void LRU();
void displayFrameTable();
int main() {

    sharedMemoryConfig();
    messageQueueConfig();
    signal(SIGINT, sigint);     // for ctrl-c termination
    signal(SIGSEGV, sigint);    // for seg faults
    initRandomForkTimes();
    initDirtyBits();

    // ##### MAIN LOOP #####
    while(procsRunning == 0) {

        // fork processes if time is reached and
        // pidHolder is not all 1's
        createProcess();

        // check for next message
        checkMsgQ();

        // do the prcocesses work
        // incremement the clock
        memoryManagement();

        // check if any of the max run counts have been met
        // if so place a 1 in the pidHolder position
        runCountCheckForTermination();

    }

    cleanup();

    return 0;
}

// handles control c
void sigint(int a) {
    // cleanup
    cleanup();

    printf("^C caught\n");
    exit(0);
}

void initDirtyBits(){
    int ii;
    for(ii = 0; ii < 256; ii++){
        frameTable.dirtyBit[ii] = '0';
    }
}

void checkMsgQ(){
    int pidPass;

    // msgrcv to receive message
    msgrcv(msgid, &message, sizeof(message), 1, IPC_NOWAIT);

    // display the message

    char *p;
    PID = strtol(message.mesg_text, &p, 10);

    strcpy(message.mesg_text, "0");


}

void cleanup(){

    if ( msgctl(msgid,IPC_RMID,0) < 0 )
    {
        perror("msgctl");
    }

    // kill open forks
    int ii;
    for(ii = 0; ii < 18; ii++){
        if(PIDHolder[ii] != 0){
            signal(SIGQUIT, SIG_IGN);
            kill(PIDHolder[ii], SIGQUIT);
        }
    }

    // clean shared memory
    shmdt(sharedShmptr);
    shmctl(sharedShmid, IPC_RMID, NULL);


}

void createProcess(){

    int ii;
    // fork into the pidholder postions with arr[pos] = 0;
    for(ii = 0; ii < 18; ii++){
        if(PIDHolder[ii] == 0){

            int positionPID = ii;

            char stashbox[10];   // pass to execl

            sprintf(stashbox, "%d", positionPID);

            // creates process in the pidHolder at
            if ((PIDHolder[ii] = fork()) == 0) {
                // argv{0] is page table number
                execl("./user", "user", stashbox, NULL);
            }



            break;
        }

    }
}

void runCountCheckForTermination(){

    //check run counts
    int ii;
    for(ii = 0; ii < 18; ii++){
        if(sharedShmptr -> processCallCount[ii] >= 10)
            PIDHolder[ii] = 1;
    }

    int sum = 0;
    //terminate if all 1's
    for(ii = 0; ii < 18; ii++){
        if(PIDHolder[ii] == 1)
            sum++;
    }

    if(sum >= 18)
        procsRunning = 1;
}

void memoryManagement(){

    int ii;

    // gets arr pos of PID in ii
    // THIS FUNCTION GETS THE ARRAY POSITION TO BE WORKED ON FOR THE ENTIRE FUCNTION
    for(ii = 0; ii < 18; ii++){
        if(PIDHolder[ii] == PID) {

            // increment clock depending on read or write
            // and write read/write to log
            if(sharedShmptr->processReadOrWrite[ii] == 0){
                theClock.milliseconds += 10;
                writeLog(1, ii, 0);
            } else if (sharedShmptr->processReadOrWrite[ii] == 1){
                theClock.milliseconds += 50;
                writeLog(2, ii, 0);
            }

            //page table check and write to log
            checkPageTable(ii);

            break;
        }
    }

    // write to pidHolder 0 after process has been handled
    for(ii = 0; ii < 18; ii++){
        if(PIDHolder[ii] == PID) {
            PIDHolder[ii] = 0;
            break;
        }
    }
}

void writeLog(int option, int procNum, int extra){

    numWritesToLog++;

    if(numWritesToLog % 10 == 0){
        //displayFrameTable();

    }

    FILE *fp = fopen("log.txt", "a+");

    switch(option){
        case 1:
            fprintf(fp, "P%d requesting read of address %d at time 0:%d\n", procNum,
                    sharedShmptr->processAddressCalled[procNum], theClock.milliseconds);
            break;

        case 2:
            fprintf(fp, "P%d requesting write of address %d at time 0:%d\n", procNum,
                    sharedShmptr->processAddressCalled[procNum], theClock.milliseconds);
            break;

        case 3:
            fprintf(fp, "Address %d is not in a frame, pagefault \n", sharedShmptr->processAddressCalled[procNum]);
            break;

        case 4:
            fprintf(fp, "Address %d wrote to frame %d\n", sharedShmptr->processAddressCalled[procNum], extra);
            break;

        case 5:
            fprintf(fp, "Dirty bit of frame %d set, adding additional time to the clock\n", extra);
            break;

        case 6:
            fprintf(fp, "Address %d is in the page %d \n", sharedShmptr->processAddressCalled[procNum], extra);
            break;

    
    }


    fclose(fp);

}

void checkPageTable(int procNum){

    //find page
    int pageNumber;
    pageNumber = sharedShmptr->processAddressCalled[procNum] / 1000;

    if(pageTable[procNum].pages[pageNumber] == 0){
        writeLog(3, procNum, 0);

        searchFTable(procNum, pageNumber);//search the frame table for a page number position

    } else if (pageTable[procNum].pages[pageNumber] != 0){
        writeLog(6, procNum, pageNumber);
    }

   

}

void searchFTable(int procNum, int pageNumber){

    int i;
    for(i = 0; i < 256; i++){
        if(frameTable.frames[i] == 0){
            frameTable.frames[i] = pageNumber;
            pageTable[procNum].pages[pageNumber] = i;
            writeLog(4, procNum, i);

            //flip dirty bit if need
            if(sharedShmptr->processReadOrWrite[procNum] == 1) {
                frameTable.dirtyBit[i] = 1;
                writeLog(5, procNum, pageNumber);
            }

        } else if (frameTable.frames[i] != 0) {
            //run LRU algorithm
            LRU();
        }

    }
}

void LRU(){
    int num = rand() % 255;
    frameTable.referenceFlag[num] = 1;
    frameTable.dirtyBit[num] = 1;
    frameTable.referenceFlag[num / 6] = 0;
    frameTable.dirtyBit[num / 6] = 0;

}

//display the frame table
void displayFrameTable(){

    FILE *fp = fopen("log.txt", "a+");

    fprintf(fp, "\n");
	fprintf(fp, "---------------------------------------------\n");
			
	fprintf(fp, "|           | Occupied | RefByte | DirtyBit |\n");		
		
	fprintf(fp, "---------------------------------------------\n");
			
							
	int i;
	for(i = 0; i < 256; i++){
		fprintf(fp, "\n");
		fprintf(fp, "| Frame %3d |",i);
				
		if(frameTable.occupied[i] == 0){
			fprintf(fp, "    No    |");
		} else {			
			fprintf(fp, "   Yes    |");
		}
		fprintf(fp,"  %3d    |",frameTable.referenceFlag[i]);
		fprintf(fp,"    %d     |",frameTable.dirtyBit[i]);
		fprintf(fp, "\n");
		}
		fprintf(fp, "---------------------------------------------\n");
				

}
