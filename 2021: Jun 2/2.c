// Program u beskonacoj petlji ceka signal, ako dobije SIGUSR1 ipsisuje apsolutnu
// vrednost broja, SIGUSR2 kvadrat, SIGTERM prekida petlju
#define _GNU_SOURCE
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

bool absolute = false;
bool square = false;
bool terminate = false;

void OsHandlerSIGUSR1(int signum){
    absolute = true;
}

void OsHandlerSIGUSR2(int signum){
    square = true;
}

void OsHandlerSIGTERM(int signum){
    terminate = true;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    check_error(signal(SIGUSR1, OsHandlerSIGUSR1) != SIG_ERR, "SIGUSR1");
    check_error(signal(SIGUSR2, OsHandlerSIGUSR2) != SIG_ERR, "SIGUSR2");
    check_error(signal(SIGTERM, OsHandlerSIGTERM) != SIG_ERR, "SIGTERM");

    do{

        pause();

        int num;
        scanf("%d", &num);

        if(absolute){
            printf("%d ", abs(num));
            absolute = false;
        }
        if(square){
            printf("%d ", num*num);
        }

    } while(!terminate);

    printf("\n");

    exit(EXIT_SUCCESS);
}
