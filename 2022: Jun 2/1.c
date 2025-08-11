// Program obilazi direktorijum sa nftw. Ako njegov naziv pocinje sa test_ i ako
// grupe i ostali korisnici nemaju prava pristupa za pisanje ipisati naziv tog
// direktorijuma.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <ftw.h>

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

int fn(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf){

    // Moze da se koristi ftwbuf->base jer je to offset od putanje do fajla
    if(typeflag == FTW_D){
        if(!(sb->st_mode & S_IWGRP) && !(sb->st_mode & S_IWOTH)){
            char* tmp = strrchr(fpath, '/');
            // if(!strncmp("test_", fpath + ftwbuf->base, 5)){
            //   printf("%s\n", fpath + ftwbuf->base); }
            if(tmp != NULL && !strncmp(tmp + 1, "test_", 5)){
                printf("%s\n", tmp + 1);
            }
        }
    }

    return 0;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "stat");
    check_error(S_ISDIR(fInfo.st_mode), "Nije dir");

    check_error(nftw(argv[1], fn, 50, 0) != -1, "nftw");

    exit(EXIT_SUCCESS);
}
