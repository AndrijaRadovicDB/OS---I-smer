// Program ceka da dobije signal, zatim ucitava dva broja. Ako je dobio SIGUSR1
// radi XOR, SIGUSR2 AND, SIGINT OR, SIGTERM izlazi iz petlje
#define _XOPEN_SOURCE 700
#define _GNU_SOURCE
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

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

bool xor = false;
bool and = false;
bool or = false;
bool terminate = false;

void OsSignalHandler(int signum){

    switch(signum){
        case SIGUSR1:
            xor = true;
            break;
        case SIGUSR2:
            and = true;
            break;
        case SIGINT:
            or = true;
            break;
        case SIGTERM:
            terminate = true;
            break;
    }

}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    check_error(signal(SIGUSR1, OsSignalHandler) != SIG_ERR, "SIGUSR1");
    check_error(signal(SIGUSR2, OsSignalHandler) != SIG_ERR, "SIGUSR2");
    check_error(signal(SIGINT, OsSignalHandler) != SIG_ERR, "SIGINT");
    check_error(signal(SIGTERM, OsSignalHandler) != SIG_ERR, "SIGTERM");

    do{

        pause();

        int a, b;
        scanf("%d%d", &a, &b);

        if(xor){
            printf("%d ", a ^ b);
            xor = false;
        }
        else if(and){
            printf("%d ", a & b);
            and = false;
        }
        else if(or){
            printf("%d ", a | b);
            or = false;
        }

    } while(!terminate);

    exit(EXIT_SUCCESS);
}
