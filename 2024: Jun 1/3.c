// Promenjiva okruzenja PATH sadrzi spisak putanja. Program treba da ispise sve
// nazive direktorijuma iz PATH za koje korisnik, grupa i ostali imaju pravo
// citanja. Putanje u formatu: <path_1>:<path_2>:...:<path_n>
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

extern char** environ;

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    char* value = getenv("PATH");
    check_error(value != NULL, "getenv");

    char* token = strtok(value, ":");
    while(token != NULL){
        char* tmp = malloc((strlen(token) + 1)*sizeof(char));
        check_error(tmp != NULL, "malloc");
        strcpy(tmp, token);

        struct stat fInfo;
        check_error(stat(tmp, &fInfo) != -1, "stat");
        if(S_ISDIR(fInfo.st_mode) && (fInfo.st_mode & S_IROTH) && (fInfo.st_mode & S_IRGRP) && (fInfo.st_mode & S_IROTH)){
            printf("%s ", tmp);
        }

        free(tmp);

        // Prvi argument je NULL jer ako parsiramo isti string on mora biti NULL
        token = strtok(NULL, ":");
    }
    printf("\n");

    exit(EXIT_SUCCESS);
}
