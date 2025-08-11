// Program dobija simbolicki link. Program ipisuje tip fajla i njegovu ekstenziju
// na koji link pokazuje, u slucaju da nema ekstenziju ispisuje nema.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

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

#define MAX_SIZE 1024

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    // Ovo je jer se prosledjuje link pa proveravamo da li je ispravan, i sa
    // readlink ga citamo, a on vraca broj procitanih bajtova
    struct stat lInfo;
    check_error(lstat(argv[1], &lInfo) != -1, "lstat");
    check_error(S_ISLNK(lInfo.st_mode), "Nije link");
    char buffer[MAX_SIZE];
    check_error(readlink(argv[1], buffer, MAX_SIZE) != -1);

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "stat");

    if(S_ISBLK(fInfo.st_mode)){
        printf("blok ");
    }
    else if(S_ISCHR(fInfo.st_mode)){
        printf("karakter ");
    }
    else if(S_ISDIR(fInfo.st_mode)){
        printf("direktorijum ");
    }
    else if(S_ISFIFO(fInfo.st_mode)){
        printf("pajp ");
    }
    else if(S_ISREG(fInfo.st_mode)){
        printf("regularan ");
    }
    else if(S_ISLNK(fInfo.st_mode)){
        printf("link ");
    }
    else if(S_ISSOCK(fInfo.st_mode)){
        printf("soket ");
    }

    char* tmp = strrchr(argv[1], '.');
    if(tmp != NULL){
        printf("%s\n", tmp);
    }
    else{
        printf("nema\n");
    }

    exit(EXIT_SUCCESS);
}
