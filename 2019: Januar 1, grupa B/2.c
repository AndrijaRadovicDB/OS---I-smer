// Program u beskonacnoj petlji ceka na signal, na pocetku je skor 0, SIGUSR1 +1,
// SIGUSR2 +2, SIGINT -4, SIGTERM izlazak iz petlje, na kraju ispisati skor.
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

bool plus1 = false;
bool plus2 = false;
bool minus4 = false;
bool terminate = false;

void osHandlerSIGUSR1(int signum){
    plus1 = true;
}

void osHandlerSIGUSR2(int signum){
    plus2 = true;
}

void osHandlerSIGINT(int signum){
    minus4 = true;
}

void osHandlerSIGTERM(int signum){
    terminate = true;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    int score = 0;

    check_error(signal(SIGUSR1, osHandlerSIGUSR1) != SIG_ERR, "signal");
    check_error(signal(SIGUSR2, osHandlerSIGUSR2) != SIG_ERR, "signal");
    check_error(signal(SIGINT, osHandlerSIGINT) != SIG_ERR, "signal");
    check_error(signal(SIGTERM, osHandlerSIGTERM) != SIG_ERR, "signal");

    do{
        pause();

        if(plus1){
            score += 1;
            plus1 = false;
        }
        if(plus2){
            score += 2;
            plus2 = false;
        }
        if(minus4){
            score -= 4;
            minus4 = false;
        }

    } while(!terminate);

    printf("%d\n", score);

    exit(EXIT_SUCCESS);
}
