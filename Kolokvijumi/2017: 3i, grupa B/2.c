// Program ispisuje velicinu fajla u KB, MB ili GB
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "Nepostojeci fajl");

    char* ime = strrchr(argv[1], '/');
    if(ime != NULL){
        ime = ime + 1;
    }
    else{
        ime = argv[1];
    }
    char* ime_velicine;

    unsigned velicina;
    if(!strcmp("KB", argv[2])){
       velicina = 1024;
       ime_velicine = argv[2];
    }
    else if(!strcmp("MB", argv[2]), argv[2]){
        velicina = 1024*1024;
        ime_velicine = argv[2];
    }
    else if(!strcmp("GB", argv[2])){
        velicina = 1024*1024*1024;
        ime_velicine = argv[2];
    }
    else{
        check_error(0, "Losa opcija");
    }
    printf("%s %u%s\n", ime, (unsigned)ceil((fInfo.st_size/(velicina*1.0))), ime_velicine);

    exit(EXIT_SUCCESS);
}
