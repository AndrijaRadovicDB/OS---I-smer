// Program vraca razliku velicine fajla na koji pokazuje simbolicki link i simbolickog linka
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

    struct stat fInfo1;
    check_error(lstat(argv[1], &fInfo1) != -1, "Nepostojeci fajl");
    check_error(S_ISLNK(fInfo1.st_mode), "Nije simbolicki link");
    struct stat fInfo2;
    check_error(stat(argv[1], &fInfo2) != -1, "Nepostojeci fajl");

    printf("%ld\n", fInfo2.st_size - fInfo1.st_size);

    exit(EXIT_SUCCESS);
}
