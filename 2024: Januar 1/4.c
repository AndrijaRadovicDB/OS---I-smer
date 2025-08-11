// Program dobija broj sekundi od epohe, a kroz promenjivu okruzenja PUTANJA
// dobija putanju do fajla. Program u beskonacnoj petlji ceka signal. SIGUSR1 -
// ispisuju se dozvole korisnika, SIGUSR2 - postavlja se vreme pristupa i
// modifikacije na prosledjeno vreme, SIGQUIT - izlazak iz petlje.
#define _XOPEN_SOURCE 700
#define _GNU_SOURCE
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <signal.h>
#include <utime.h>

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

extern char** environ;

bool check_permissions = false;
bool change_time = false;
bool terminate = false;

void* OsSignalHandlerSIGUSR1(int signum){
    (void)signum;
    check_permissions = true;
}

void* OsSignalHandlerSIGUSR2(int signum){
    (void)signum;
    change_time = true;
}

void* OsSignalHandlerSIGQUIT(int signum){
    (void)signum;
    terminate = true;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    time_t change_time = atoi(argv[1]);

    check_error(signal(SIGUSR1, OsSignalHandlerSIGUSR1) != SIG_ERR, "SIGUSR1");
    check_error(signal(SIGUSR2, OsSignalHandlerSIGUSR1) != SIG_ERR, "SIGUSR2");
    check_error(signal(SIGQUIT, OsSignalHandlerSIGQUIT) != SIG_ERR, "SIGQUIT");

    char* value = getenv("PUTANJA");
    check_error(value != NULL, "getenv");
    struct stat fInfo;
    check_error(stat(value, &fInfo) != -1, "stat");

    do{

        pause();

        if(check_permissions){
            if(fInfo.st_mode & I_IRUSR){
                printf("r");
            }
            else{
                printf("-");
            }
            if(fInfo.st_mode & I_IWUSR){
                printf("w");
            }
            else{
                printf("-");
            }
            if(fInfo.st_mode & I_IXUSR){
                printf("x");
            }
            else{
                printf("-");
            }
            check_permissions = false;
        }

        if(change_time){
            struct utimebuf new_time;
            new_time.actime = change_time;
            new_time.modtime = change_time;
            // utime postavlja atime i mtime datog fajla
            check_error(utime(value, &new_time) != -1, "utime");
            change_time = false;
        }

    } while(!terminate);

    exit(EXIT_SUCCESS);
}
