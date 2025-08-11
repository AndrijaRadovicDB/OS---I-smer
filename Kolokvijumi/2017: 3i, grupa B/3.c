// Program pravi fajl sa datim pravima pristupa
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

    errno = 0;
    mode_t mode = strtol(argv[3], NULL, 8);
    mode_t oldUmask = umask(0);

    if(!strcmp("-f", argv[1])){
        // O_CREAT | O_EXCL failuje ako fajl vec postoji
        int fd = open(argv[2], O_CREAT | O_EXCL, mode);
        check_error(fd != -1, "open");
        close(fd);
    }
    else if(!strcmp("-d", argv[1])){
        int napravi = mkdir(argv[2], mode);
        if(napravi < 0 || (napravi < 0 && errno == EEXIST)){
            check_error(napravi != -1, "Los fajl deskriptor");
        }
        check_error(chmod(argv[2], mode) != -1, "chmod");
    }
    else{
        check_error(0, "Pogresna opcija");
    }

    umask(oldUmask);

    exit(EXIT_SUCCESS);
}
