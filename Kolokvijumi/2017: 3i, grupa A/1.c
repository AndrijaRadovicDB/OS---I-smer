// Program za prosledjenu putanju ispisuje tip fajla i prava pristupa
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

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) !=-1, "Nepostojeci fajl");

    char* fInfoStr = malloc(MAX_SIZE);
    char* prava_pristupa = "-rwxrwxrwx";
    strcpy(fInfoStr, prava_pristupa);

    if((fInfo.st_mode & S_IFMT) == S_IFDIR){
        fInfoStr[0] = 'd';
    }
    else if((fInfo.st_mode & S_IFMT) == S_IFREG){
        fInfoStr[0] = '-';
    }

    if(!(fInfo.st_mode & S_IRUSR)){
       fInfoStr[1] = '-';
    }
    if(!(fInfo.st_mode & S_IWUSR)){
        fInfoStr[2] = '-';
    }
    if(!(fInfo.st_mode & S_IXUSR)){
        fInfoStr[3] = '-';
    }
    if(!(fInfo.st_mode & S_IRGRP)){
        fInfoStr[4] = '-';
    }
    if(!(fInfo.st_mode & S_IWGRP)){
        fInfoStr[5] = '-';
    }
    if(!(fInfo.st_mode & S_IXGRP)){
        fInfoStr[6] = '-';
    }
    if(!(fInfo.st_mode & S_IROTH)){
        fInfoStr[7] = '-';
    }
    if(!(fInfo.st_mode & S_IWOTH)){
        fInfoStr[8] = '-';
    }
    if(!(fInfo.st_mode & S_IXOTH)){
        fInfoStr[9] = '-';
    }

    printf("%s\n", fInfoStr);

    free(fInfoStr);

    exit(EXIT_SUCCESS);
}
