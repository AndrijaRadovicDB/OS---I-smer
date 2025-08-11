// Program vrsi zamenu prava pristupa za vlasnicku grupu i ostale korisnike
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
    check_error(stat(argv[1], &fInfo) != -1, "Nepostojeci fajl");
    check_error(!S_ISDIR(fInfo.st_mode), "Nije regularni fajl");

    mode_t prava_pristupa = 0;
    if(fInfo.st_mode & S_IRUSR){
        prava_pristupa |= S_IRUSR;
    }
    if(fInfo.st_mode & S_IWUSR){
        prava_pristupa |= S_IWUSR;
    }
    if(fInfo.st_mode & S_IXUSR){
        prava_pristupa |= S_IXUSR;
    }

    mode_t prava_pristupa_grupa = 0;
    if(fInfo.st_mode & S_IRGRP){
        prava_pristupa_grupa |= S_IRGRP;
    }
    if(fInfo.st_mode & S_IWGRP){
        prava_pristupa_grupa |= S_IWGRP;
    }
    if(fInfo.st_mode & S_IXGRP){
        prava_pristupa_grupa |= S_IXGRP;
    }
    prava_pristupa_grupa >>= 3;

    mode_t prava_pristupa_ostalih = 0;
    if(fInfo.st_mode & S_IROTH){
        prava_pristupa_ostalih |= S_IROTH;
    }
    if(fInfo.st_mode & S_IWOTH){
        prava_pristupa_ostalih |= S_IWOTH;
    }
    if(fInfo.st_mode & S_IXOTH){
        prava_pristupa_ostalih |= S_IXOTH;
    }
    prava_pristupa_ostalih <<= 3;

    prava_pristupa |= prava_pristupa_ostalih;
    prava_pristupa |= prava_pristupa_grupa;

    check_error(chmod(argv[1], prava_pristupa) != -1, "chmod");

    exit(EXIT_SUCCESS);
}
