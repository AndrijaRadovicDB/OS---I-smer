// Program menja prava pristupa 2 fajla v2 -> g1, g2 -> o1, o2 -> v1
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

    check_error(argc == 3, "Argumenti");

    struct stat fInfo1;
    check_error(stat(argv[1], &fInfo1) != -1, "Nepostojeci fajl");
    struct stat fInfo2;
    check_error(stat(argv[2], &fInfo2) != -1, "Nepostojeci fajl");

    mode_t prava_vlasnika = 0;
    if(fInfo1.st_mode & S_IRUSR){
        prava_vlasnika |= S_IRUSR;
    }
    if(fInfo1.st_mode & S_IWUSR){
        prava_vlasnika |= S_IWUSR;
    }
    if(fInfo1.st_mode & S_IXUSR){
        prava_vlasnika |= S_IXUSR;
    }
    prava_vlasnika >>= 6;

    mode_t prava_grupe = 0;
    if(fInfo1.st_mode & S_IRGRP){
        prava_grupe |= S_IRGRP;
    }
    if(fInfo1.st_mode & S_IWGRP){
        prava_grupe |= S_IWGRP;
    }
    if(fInfo1.st_mode & S_IXGRP){
        prava_grupe |= S_IXGRP;
    }
    prava_grupe <<= 3;

    mode_t prava_ostalih = 0;
    if(fInfo1.st_mode & S_IROTH){
        prava_ostalih |= S_IROTH;
    }
    if(fInfo1.st_mode & S_IWOTH){
        prava_ostalih |= S_IWOTH;
    }
    if(fInfo1.st_mode & S_IXOTH){
        prava_ostalih |= S_IXOTH;
    }
    prava_ostalih <<= 3;

    mode_t prava_drugog = 0;
    prava_drugog |= prava_vlasnika | prava_grupe | prava_ostalih;
    check_error(chmod(argv[2], prava_drugog) != -1, "chmod");

    exit(EXIT_SUCCESS);
}
