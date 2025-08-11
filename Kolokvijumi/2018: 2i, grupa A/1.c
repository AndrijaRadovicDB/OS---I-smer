// Program menja vreme pristupa i modifikacije
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <time.h>
#include <utime.h>
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

    check_error(argc == 3, "Argumenti");

    time_t vreme = strtol(argv[2], NULL, 10);
    struct utimbuf novo_vreme;
    novo_vreme.actime = vreme;
    novo_vreme.modtime = vreme;
    check_error(utime(argv[1], &novo_vreme) != -1, "utime");

    exit(EXIT_SUCCESS);
}
