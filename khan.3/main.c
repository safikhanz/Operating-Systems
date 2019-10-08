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

	int opt; 
	char filename[256]; 
	int maxChildProcess = 5;   
	int forkCount =0;
	int killTime =20; 
	int i,j; 
	int clockSize[3];

	int shmid = shmget ( SHMKEY, sizeof(clockSize[3]), 0777 | IPC_CREAT );
	
 	char * paddr = ( char * )( shmat ( shmid, NULL, 0 ) );
	int * pint = ( int * )(paddr);
	pint[1]=0; 
	pint[2]=0; 
	pint[3]=0; 
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
	alarm(killTime); 
	forkCount = maxChildProcess;
	pid_t pidHolder[maxChildProcess];
	for(i=0; i<maxChildProcess; i++){
		if ((pidHolder[i] =fork())==0){
		  execl("./user", "user", NULL);
		}
	}

	while(1 && flag ==0){

		
		pint[2]+=30000; 	
		if(pint[2] > 999999999){
		
		pint[1]+=1;
		pint[2] =0; 
		}
		
		if(forkCount == 100){
			printf("Program terminated after 100 forks\n");
			
		
			wait(0);
		
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
