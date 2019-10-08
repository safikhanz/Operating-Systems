#include<stdio.h> 
  
int i; // for loop  
int main() 
{ 
    for(i=0;i<5;i++) // loop will run n times (n=5) 
    { 
        if(fork() == 0) 
        { 
            printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid()); 
            exit(0); 
        } 
    } 
    for(i=0;i<5;i++) // loop will run n times (n=5) 
    wait(NULL); 
      
} 

