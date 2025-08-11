// Program menja prava pristupa vlasnik -> ostali, grupa -> vlasnik, ostali -> grupa
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

    mode_t prava_pristupa = 0;
    mode_t prava_vlasnik = 0, prava_grupe = 0, prava_ostalih = 0;

    if(fInfo.st_mode & S_IRUSR){
        prava_vlasnik |= S_IRUSR;
    }
    if(fInfo.st_mode & S_IWUSR){
        prava_vlasnik |= S_IWUSR;
    }
    if(fInfo.st_mode & S_IXUSR){
        prava_vlasnik |= S_IXUSR;
    }
    prava_vlasnik >>= 6;

    if(fInfo.st_mode & S_IRGRP){
        prava_grupe |= S_IRGRP;
    }
    if(fInfo.st_mode & S_IWGRP){
        prava_grupe |= S_IWGRP;
    }
    if(fInfo.st_mode & S_IXGRP){
        prava_grupe |= S_IXGRP;
    }
    prava_grupe <<= 3;

    if(fInfo.st_mode & S_IROTH){
        prava_ostalih |= S_IROTH;
    }
    if(fInfo.st_mode & S_IWOTH){
        prava_ostalih |= S_IWOTH;
    }
    if(fInfo.st_mode & S_IXOTH){
        prava_ostalih |= S_IXOTH;
    }
    prava_ostalih <<= 3;

    prava_pristupa |= prava_vlasnik | prava_grupe | prava_ostalih;
    check_error(chmod(argv[1], prava_pristupa) != -1, "chmod");

    exit(EXIT_SUCCESS);
}
