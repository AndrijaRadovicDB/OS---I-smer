// Program u beskonacnoj petlji ceka na signal i onda ucitava ceo broj. Ako je
// signal SIGUSR1 program ispisuje aps vrednost broja, ako je SIGUSR2 ispisuje
// kvadrat broja, za SIGTERM izlazi iz petlje
#define _XOPEN_SORUCE 700
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

bool absl = false;
bool square = false;
bool terminate = false;

void osHandlerSIGUSR1(int signum){
    absl = true;
}

void osHandlerSIGUSR2(int signum){
    square = true;
}

void osHandlerSIGTERM(int signum){
    terminate = true;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    check_error(signal(SIGUSR1, osHandlerSIGUSR1) != SIG_ERR, "signal");
    check_error(signal(SIGUSR2, osHandlerSIGUSR2) != SIG_ERR, "signal");
    check_error(signal(SIGTERM, osHandlerSIGTERM) != SIG_ERR, "signal");

    do{
        pause();

        int num;
        scanf("%d", &num);

        if(absl){
            num = abs(num);
            printf("%d\n", num);
            absl = false;
        }
        if(square){
            num = num*num;
            printf("%d\n", num);
            square = false;
        }

    } while(!terminate);

    exit(EXIT_SUCCESS);
}
