// Program ispisuje ime dana u nedelji i redni broj dana u mesecu koji se dobije
// kada se na danasnje vreme doda prosledjen broj dana
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <time.h>

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

void dan_u_nedelji(char* nedelja, struct tm* broken_time){

    int indeks = broken_time->tm_wday;

    if(indeks == 0){
        strcpy(nedelja, "Nedelja");
    }
    else if(indeks == 1){
        strcpy(nedelja, "Ponedeljak");
    }
    else if(indeks == 2){
        strcpy(nedelja, "Utorak");
    }
    else if(indeks == 3){
        strcpy(nedelja, "Sreda");
    }
    else if(indeks == 4){
        strcpy(nedelja, "Cetvrtak");
    }
    else if(indeks == 5){
        strcpy(nedelja, "Petak");
    }
    else if(indeks == 6){
        strcpy(nedelja, "Subota");
    }
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    int broj_dana = atoi(argv[1]);
    broj_dana *= 24*60*60;

    time_t now = time(NULL);
    now += broj_dana;

    struct tm* broken_time = localtime(&now);
    check_error(broken_time != NULL, "localtime");
    char nedelja[15];

    dan_u_nedelji(nedelja, broken_time);

    printf("%s %d\n", nedelja, broken_time->tm_mday);

    exit(EXIT_SUCCESS);
}
