#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/type.h>
#include<sys/shm.h>

int main(int argc, char* argv[])
{
        int opt;
        char filename;
        char *topdir;
        int n;
        while((opt = getopt(argc, argv, "hs:l:t:z")) != -1)
        {
                switch(opt)
                {
                        case 'h':
                        break;
                        case 's';
                        break;
                        case 'l'
