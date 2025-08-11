// Program u beskonacnoj petlji motri SIGUSR1, SIGUSR2, SIGTERM, za prva 2 broji
// koliko su se puta desili, a za 3. prekida program i ispisuje broj prva 2
#define _DEFAULT_SOURCE // Ovo mora za signale
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

#define UNUSED_FUNC_ARG(x) ((void)x)

static bool osShouldTerminate = false;
int broj_sigusr1 = 0;
int broj_sigusr2 = 0;

// Umesto 3 signal handlera mogao je 1 veliki singal handler
void osHandleSIGTERM(int signum){
    UNUSED_FUNC_ARG(signum);
    printf("%d %d\n", broj_sigusr1, broj_sigusr2);
    osShouldTerminate = true;
}

void osHandleSIGUSR1(int signum){
    UNUSED_FUNC_ARG(signum);
    broj_sigusr1++;
}

void osHandleSIGUSR2(int signum){
    UNUSED_FUNC_ARG(signum);
    broj_sigusr2++;
}

int main(int argc, char** argv){

    // Postavljanje signal handlera
    check_error(SIG_ERR != signal(SIGUSR1, osHandleSIGUSR1), "SIGUSR1 handler");
    check_error(SIG_ERR != signal(SIGUSR2, osHandleSIGUSR2), "SIGUSR2 handler");
    check_error(SIG_ERR != signal(SIGTERM, osHandleSIGTERM), "SIGTERM handler");

    do{
        // Program ceka na signal
        pause();
        // Deo nakon pause() se izvrsava nakon sto stigne signal i singal handler ga obradi
    }
    while(!osHandleSIGTERM);

    exit(EXIT_SUCCESS);
}
