// Program u beskonacnoj petlji ceka na signal, za SIGUSR1 skor + 1, SIGUSR2 + 2
// SIGINT - 4, SIGTERM izlaz iz petlje
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

bool add1 = false;
bool add2 = false;
bool sub4 = false;
bool terminate = false;

void OsHandlerSIGUSR1(int signum){
    (void)signum;
    add1 = true;
}

void OsHandlerSIGUSR2(int signum){
    (void)signum;
    add2 = true;
}

void OsHandlerSIGINT(int signum){
    (void)signum;
    sub4 = true;
}

void OsHandlerSIGTERM(int signum){
    (void)signum;
    terminate = true;
}


int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    check_error(signal(SIGUSR1, OsHandlerSIGUSR1) != SIG_ERR, "SIGUSR1");
    check_error(signal(SIGUSR2, OsHandlerSIGUSR2) != SIG_ERR, "SIGUSR2");
    check_error(signal(SIGINT, OsHandlerSIGINT) != SIG_ERR, "SIGINT");
    check_error(signal(SIGTERM, OsHandlerSIGTERM) != SIG_ERR, "SIGTERM");

    int score = 0;
    do{

        pause();

        if(add1){
            score += 1;
            add1 = false;
        }
        if(add2){
            score += 2;
            add2 = false;
        }
        if(sub4){
            score -= 4;
            sub4 = false;
        }

    } while(!terminate);
    printf("%d\n", score);

    exit(EXIT_SUCCESS);
}
