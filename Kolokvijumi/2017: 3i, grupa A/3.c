// Program na prosledjenoj putanji kreira fajl datog tipa sa datim pravima
// pristupa, a ako fajl vec postoji, treba mu samo promeniti prava pristupa
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

int main(int argc, char** argv){

    check_error(argc == 4, "Argumenti");

    mode_t mode = 0777;

    if(!strcmp("-d", argv[1])){
        int napravi = mkdir(argv[2], mode);
        if((napravi < 0 && errno == EEXIST) || napravi >= 0){
            mode = strtol(argv[3], NULL, 8);
            check_error(chmod(argv[2], mode) != -1, "chmod");
        }
    }
    else if(!strcmp("-f", argv[1])){
        mode = strtol(argv[3], NULL, 8);
        int fd = open(argv[2], O_CREAT, mode);
        check_error(fd != -1, "open");
        close(fd);
    }

    exit(EXIT_SUCCESS);
}
