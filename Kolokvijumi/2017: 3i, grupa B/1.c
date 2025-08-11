// Program ispisuje razliku vremena posledenjeg pristupa i modifikacije u danima
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <time.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "Nepostojeci fajl");
    check_error(S_ISREG(fInfo.st_mode), "Nije regularan fajl");

    time_t razlika = fInfo.st_atime - fInfo.st_mtime;
    printf("%u\n", (unsigned)(razlika/(24*60*60)));

    exit(EXIT_SUCCESS);
}
