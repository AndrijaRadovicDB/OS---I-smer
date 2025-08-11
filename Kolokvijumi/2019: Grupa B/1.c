// Program ispisuje naziv fajla koji je skorije modifikovan
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

    check_error(argc == 3, "Argumenti");

    struct stat fInfo1;
    check_error(stat(argv[1], &fInfo1) != -1, "Nepostojeci fajl");
    struct stat fInfo2;
    check_error(stat(argv[2], &fInfo2) != -1, "Nepostojeci fajl");

    char* niska = malloc(sizeof(char)*MAX_SIZE + 1);
    check_error(niska != NULL, "malloc");

    if(fInfo1.st_mtime > fInfo2.st_mtime){
        char* postoji = strrchr(argv[1], '/');
        int duzina;
        if(postoji != NULL){
            duzina = (int)strlen(postoji);
        }
        else{
            duzina = (int)strlen(argv[1]);
        }

        for(int i = 0; i < duzina; i++){
            if(postoji != NULL){
                niska[i] = postoji[i + 1];
            }
            else{
                niska[i] = argv[1][i];
            }
        }
        niska[duzina] = '\0';
    }
    else{
        char* postoji = strrchr(argv[2], '/');
        int duzina;
        if(postoji != NULL){
            duzina = (int)strlen(postoji);
        }
        else{
            duzina = (int)strlen(argv[1]);
        }

        for(int i = 0; i < duzina; i++){
            if(postoji != NULL){
                niska[i] = postoji[i + 1];
            }
            else{
                niska[i] = argv[2][i];
            }
        }
        niska[duzina] = '\0';
    }
    printf("%s\n", niska);

    free(niska);

    exit(EXIT_SUCCESS);
}
