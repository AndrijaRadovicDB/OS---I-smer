// Program ucitava putanju do fajla i ceka na signal, ako dobije SIGUSR1 ispisuje
// velicinu fajla, SIGUSR2 ispisuje dan u mesecu poslednje modifikacije tog fajla,
// SIGQUIT prekida se cekanje na signale.
#define _GNU_SOURCE
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <signal.h>
#include <time.h>

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

bool print_size = false;
bool mtime_day = false;
bool terminate = false;

void OsSignalHanlderSIGUSR1(int signum){
    (void)signum;
    print_size = true;
}

void OsSignalHanlderSIGUSR2(int signum){
    (void)signum;
    mtime_day = true;
}

void OsSignalHanlderSIGQUIT(int signum){
    (void)signum;
    terminate = true;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    check_error(signal(SIGUSR1, OsSignalHanlderSIGUSR1) != SIG_ERR, "SIGUSR1");
    check_error(signal(SIGUSR2, OsSignalHanlderSIGUSR2) != SIG_ERR, "SIGUSR2");
    check_error(signal(SIGQUIT, OsSignalHanlderSIGQUIT) != SIG_ERR, "SIGQUIT");

    do{

        char* tmp;
        scanf("%ms", &tmp);

        pause();

        struct stat fInfo;
        check_error(stat(tmp, &fInfo) != -1, "stat");

        if(print_size){
            printf("%ld ", fInfo.st_size);
            print_size = false;
        }

        if(mtime_day){
            struct tm* broken_time = localtime(&fInfo.st_mtime);
            check_error(broken_time != NULL, "localtime");
            printf("%d ", broken_time->tm_mday);
            mtime_day = false;
        }

        free(tmp);

    } while(!terminate);

    printf("\n");

    exit(EXIT_SUCCESS);
}
