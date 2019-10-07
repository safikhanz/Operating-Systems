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

void helpMenu() {
		printf("---------------------------------------------------------------| Help Menu |--------------------------------------------------------------------------\n");
		printf("-h help menu\n"); 
		printf("-i inputfilename                      | inputfilename is where the filename reads and it will show error if there is no filename found on the directory.\n"); 
		printf("-n int				      | int for max processor\n"); 
		printf("------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}





int main(int argc, char* argv[])
{
 
	int opt;
	char filename[256];
	char *topdir;
	int n;
	int maxChildProc = 5;
        while((opt = getopt(argc, argv, "hs:l:t:")) != -1)
        {
                switch(opt)
                {
                        case 'h':
                        	helpMenu();
				break;
                        case 's':
				maxChildProc = atoi(optarg);
                        	break;
                        case 'l':
				strcpy(filename, optarg);
				printf("%s",filename);
				break;
			case 't':
				
				break;
		}
	}

return 0;
}
