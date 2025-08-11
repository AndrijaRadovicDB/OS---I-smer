// Program u beskonacnoj petlji ucitava reci, nakon cega ceka na signal. Ako
// dobije SIGUSR1 ispisuje indikator da li je rec palindrom, SIGUSR2 obrce rec,
// a SIGQUIT izlazi iz programa bez obrade poslednje reci.
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

#define MAX_SIZE 256

bool check_palindrom = false;
bool reverse = false;
bool terminate = false;

void OsSignalHandlerSIGUSR1(int signum){
    (void)signum;
    check_palindrom = true;
}

void OsSignalHandlerSIGUSR2(int signum){
    (void)signum;
    reverse = true;
}

void OsSignalHandlerSIGQUIT(int signum){
    (void)signum;
    terminate = true;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    do{

        // Ovo se radi u petlji jer signal handler vazi za jednu obradu signala
        // ako ne zelimo ovako ponasanje treba da ukljucimo marko
        // #define _GNU_SOURCE
        check_error(signal(SIGUSR1, OsSignalHandlerSIGUSR1) != SIG_ERR, "SIGUSR1");
        check_error(signal(SIGUSR2, OsSignalHandlerSIGUSR2) != SIG_ERR, "SIGUSR2");
        check_error(signal(SIGQUIT, OsSignalHandlerSIGQUIT) != SIG_ERR, "SIGQUIT");

        char word[MAX_SIZE];
        scanf("%s", word);

        pause();

        if(check_palindrom){
            bool is_palindrom = true;
            int n = strlen(word);
            for(int i = 0; i < n/2; i++){
                if(word[i] != word[n - i - 1]){
                    is_palindrom = false;
                    break;
                }
            }
            if(is_palindrom){
                printf("1 ");
            }
            else{
                printf("0 ");
            }
            check_palindrom = false;
        }

        if(reverse){
            int n = strlen(word);
            for(int i = 0; i < n/2; i++){
                char tmp = word[i];
                word[i] = word[n - i - 1];
                word[n - i - 1] = tmp;
            }
            printf("%s ", word);
            reverse = false;
        }

    } while(!terminate);

    printf("\n");

    exit(EXIT_SUCCESS);
}
