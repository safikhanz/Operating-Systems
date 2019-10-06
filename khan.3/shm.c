#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
void parent();
void child ();
#define SHMKEY    859047             /* Parent and child agree on common key.*/
#define BUFF_SZ   sizeof ( int )
using namespace std;
int main(){
    switch ( fork() )    {
	case -1:    cerr << "Failed to fork" << endl;    
	return ( 1 );
	case 0:    child();    
	break;
	default:    parent();    
	break;    
	}    
return ( 0 );
}
	void parent(){    
		// Get shared memory segment identifier    
		int shmid = shmget ( SHMKEY, BUFF_SZ, 0777 | IPC_CREAT );    
		if ( shmid == -1 ){ 
			cerr << "Parent: ... Error in shmget ..." << endl;
			exit (1);    
		}    // Get the pointer to shared block 
   		char * paddr = ( char * )( shmat ( shmid, 0, 0 ) );   
		int * pint = ( int * )( paddr );   
	 	for ( int i ( 0 ); i < 10; i++ ){
			sleep ( 2 );
			*pint = 10 *i ;      /* Write into the shared area. */
			cout << "Parent: Written Val.: = " << *pint << endl;    
		}
	}
void child(){ 
	sleep ( 5 );   
	int shmid = shmget ( SHMKEY, BUFF_SZ, 0777 ); 
	if ( shmid == -1 )    
		{cerr << "Child: ... Error in shmget ..." << endl;exit ( 1 );  
	  }  
 	 int * cint = ( int * )( shmat ( shmid, 0, 0 ) );  
	  for ( int i ( 0 ); i < 10; i++ ){
	sleep ( 1 );cout << "Child: Read Val. = " << *cint << endl;    
	}
}
