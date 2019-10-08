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

int flag =0;
sem_t *shm;
sem_t *shmPtr;

void helpMenu() {
		printf("\n------------| Help Menu |---------------\n");
		printf("-h help menu\n"); 
		printf("-i filename	| filename of the log file.\n"); 
		printf("-t z		| z for time to end the process\n");
		printf("-n x		| int for max processor\n"); 
		printf("-------------------------------------------\n");
}


static void alarmHandler(int signo);

int main(int argc, char* argv[])
{

	int opt;//user choice for switch 
	char filename[256]; //for log file
	int maxChildProcess = 5;  // user choice for no. of forks, default set to 5 
	int forkCount =0;
	int killTime =20; //max time until master terminates itself 
	int i,j;  // for looping 
	int clockSize[3];

	int shmid = shmget ( SHMKEY, sizeof(clockSize[3]), 0777 | IPC_CREAT );
	//get pointer to memory block
 	char * paddr = ( char * )( shmat ( shmid, NULL, 0 ) );
	int * pint = ( int * )(paddr);
	pint[1]=0; //for seconds
	pint[2]=0; // for nanoseconds
	pint[3]=0; //shmMsg
       while((opt = getopt(argc, argv, "hs:l:t:")) != -1)
        {
                switch(opt)
                {
                        case 'h':
                        	helpMenu();
				break;
                        case 's':
				maxChildProcess = atoi(optarg);
                        	break;
                        case 'l':
				strcpy(filename, optarg);
				printf("%s",filename);
				break;
			case 't':
				killTime = atoi(optarg);
				break;
		}
	}
//	signal(SIGALRM, alarmHandler);
	alarm(killTime); // alarm times out (default 2 seconds) in killTime seconds, which is provided by user. 
	forkCount = maxChildProcess;
	pid_t pidHolder[maxChildProcess];
	for(i=0; i<maxChildProcess; i++){
		if ((pidHolder[i] =fork())==0){
		  execl("./user", "user", NULL);
		}
	}

	while(1 && flag ==0){
/*
 *  			Shared clock 
 *  			 		*/
		
		pint[2]+=30000;  //increasing nanoseconds by 30000 each iteration	
		if(pint[2] > 999999999){
		//conversions for second and nanpseconds
		pint[1]+=1;
		pint[2] =0;//setting back nanoseconds after conversion 
		}
		
		if(forkCount == 100){
			printf("Program terminated after 100 forks\n");
		//parent waits for all children to finish	
			wait(0);
		//cleaning the shared memory
			shmdt(pint);
			return 0;

		} 	

		
		if(pint[2] > 0){
                for(j = 0; j < maxChildProcess;j++){
                    if(pint[2] == pidHolder[j]) {
                       
                        FILE *fp = fopen(filename, "a+");
                        fputs("Child: ", fp);
                        fprintf(fp, "%d", pidHolder[j]);
                        fputs(" is terminating at my time ", fp);
                        fprintf(fp, "%d %s %d %s", pint[0], "seconds,", pint[1], "nanoseconds.\n");
                        fclose(fp);

                        forkCount++;

                   
                        if ((pidHolder[j] = fork()) == 0)
                            execl("./user", "user", NULL);

                    }
                }

             
                pint[2] = 0;

             
            }
        }

        wait(NULL);
        
        shmdt(paddr);


        
        printf("\n end of parent \n");


        return 0;
}
static void alarmHandler(int signo){
printf("Caught an SIGALRM signal.\n");
    printf("Signal value = %d\n", signo);

    printf("Exiting from process...\n");


flag =1;
exit(EXIT_SUCCESS);

}
