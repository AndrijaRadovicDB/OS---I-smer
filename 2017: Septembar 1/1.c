// Program u petlji ucitava rec, zatim ceka da mu se prosledi signal, ako dobije
// SIGUSR1 -> ispisuje rec naopako, SIGUSR2 -> mala slova u velika, SIGTERM ->
// kraj izvrsavanja programa
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

#define UNUSED_FUNC_ARG(x) ((void) x)
#define MAX_SIZE (64)

bool terminate = false;
bool reverse = false;
bool toUpper = false;

void osHandlerSIGTERM(int signum){
    UNUSED_FUNC_ARG(signum);
    terminate = true;
}

void osHandlerSIGUSR1(int signum){
    UNUSED_FUNC_ARG(signum);
    reverse = true;
}

void osHandlerSIGUSR2(int signum){
    UNUSED_FUNC_ARG(signum);
    toUpper = true;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    do{

        check_error(SIG_ERR != signal(SIGUSR1, osHandlerSIGUSR1), "SIGUSR1");
        check_error(SIG_ERR != signal(SIGUSR2, osHandlerSIGUSR2), "SIGUSR2");
        check_error(SIG_ERR != signal(SIGTERM, osHandlerSIGTERM), "SIGTERM");
        pause();

        char word[MAX_SIZE];
        scanf("%s", word);

        if(reverse){
            for(int i = strlen(word) - 1; i >= 0; i--){
                printf("%c", word[i]);
            }
            printf("\n");
            reverse = false;
        }
        if(toUpper){
            for(int i = 0; word[i]; i++){
                if(islower(word[i])){
                    printf("%c", toupper(word[i]));
                }
                else{
                    printf("%c", word[i]);
                }
            }
            printf("\n");
            toUpper = false;
        }

    } while(!terminate);


    exit(EXIT_SUCCESS);
}
