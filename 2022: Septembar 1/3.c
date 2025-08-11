// Program u beskonacnoj petlji ucitava putanju do fajla i ceka signal, ako
// dobije SIGUSR1 ispisuje ime korisnika koji je vlasnik tog fajla, SIGQUIT tada
// iskace iz petlje.
// Ako ne zelimo signal handlere u petlji svaki put
#define _GNU_SOURCE
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <pwd.h>
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

bool print_user = false;
bool terminate = false;

void signalHanlderSIGUSR1(int signum){
    (void)signum;
    print_user = true;
}

void signalHanlderSIGQUIT(int signum){
    (void)signum;
    terminate = true;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    check_error(signal(SIGUSR1, signalHanlderSIGUSR1) != SIG_ERR, "SIGUSR1");
    check_error(signal(SIGQUIT, signalHanlderSIGQUIT) != SIG_ERR, "SIGQUIT");

    do{
        char* fPath;
        scanf("%ms", &fPath);

        pause();

        if(print_user){
            struct stat fInfo;
            check_error(stat(fPath, &fInfo) != -1, "stat");
            struct passwd* usr = getpwuid(fInfo.st_uid);
            check_error(usr != NULL, "getpwuid");
            printf("%s\n", usr->pw_name);
            print_user = false;
        }

        // Mozda treba free za %ms mozda ne
        //free(fPath);
    } while(!terminate);


    exit(EXIT_SUCCESS);
}
