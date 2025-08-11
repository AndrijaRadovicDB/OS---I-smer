// Program ipisuje vreme modifikacije simbolickog linka
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

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
    check_error(lstat(argv[1], &fInfo) != -1, "Nepostojeci fajl");
    check_error(S_ISLNK(fInfo.st_mode), "Pogresan tip");

    printf("%u\n", (unsigned)fInfo.st_mtime);

    exit(EXIT_SUCCESS);
}
