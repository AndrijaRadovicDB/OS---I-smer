// Program rekurzivno obilazi direktorijum sa nftw. Na kraju ispisuje ukupnu
// velicinu svih regularnih fajlova koji imaju execute pravo.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <ftw.h>

#include <stdio.h>
#include <stdlib.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

unsigned size = 0;

int fn(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf){

    if(typeflag == FTW_F){
        if((sb->st_mode & S_IXUSR) || (sb->st_mode & S_IXGRP) || (sb->st_mode & S_IXOTH)){
            size += sb->st_size;
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

    printf("%u\n", size);

    exit(EXIT_SUCCESS);
}
