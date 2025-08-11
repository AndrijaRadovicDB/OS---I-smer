// Program ispisuje naziv najmanjem prosledjenog fajla. Pratiti symlink.
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

int main(int argc, char** argv){

    check_error(argc > 1, "Argumenti");

    int max_size = 0;
    char* max_name = NULL;
    for(int i = 1; i < argc; i++){
        struct stat fInfo;
        int ret_val = stat(argv[i], &fInfo);
        if(ret_val != -1){
            if(fInfo.st_size > max_size){
                max_size = fInfo.st_size;
                max_name = argv[i];
            }
        }
    }

    // Ako treba da se ispise na sta pokazuje symlink, onda bi morali da uradimo
    // readlink i iz njega da izvucemo ime fajla
    if(max_size){
        char* tmp = strrchr(max_name, '/');
        if(tmp != NULL){
            printf("%s\n", tmp + 1);
        }
        else{
            printf("%s\n", max_name);
        }
    }

    exit(EXIT_SUCCESS);
}
