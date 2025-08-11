// Program menja ime direktorijuma
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

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "Nepostojeci fajl");
    check_error(S_ISDIR(fInfo.st_mode), "Dir");


    char* staro_ime = strrchr(argv[1], '/');
    if(staro_ime == NULL){
        check_error(rename(argv[1], argv[2]) != -1, "rename");
    }
    else{
        // Ovo dole je pokazivacka aritmetika, ovo gleda na pocetka obe niske
        // Kako su jedna pored druge ovo radi
        int velicina = staro_ime - argv[1] + 1;
        char* novo_ime = malloc(velicina + strlen(argv[2]) + 1);
        check_error(novo_ime != NULL, "malloc");

        strncpy(novo_ime, argv[1], velicina);
        strcat(novo_ime, argv[2]);
        check_error(rename(argv[1], novo_ime) != -1, "rename");

        free(novo_ime);
    }

    exit(EXIT_SUCCESS);
}
