// Program pravi direktorijum sa prosledjenim imenom i pravima pristupa 0777
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#define check_error(cond)\
    do{\
        if(!(cond)){\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

void osMkPublicDir(const char* dname){

    static mode_t mode = 0777;
    check_error(mkdir(dname, mode) != -1);
    check_error(chmod(dname, mode) != -1);
}

int main(int argc, char** argv){

    check_error(argc == 2);

    osMkPublicDir(argv[1]);

    exit(EXIT_SUCCESS);
}
