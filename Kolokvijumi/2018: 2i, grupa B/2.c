// Program ipsiuje sve direktorijume za koje ostali korisnici nemaju ni jedna prava pristupa
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

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

int main(int argc, char** argv){

    check_error(argc != 1, "Argumenti");

    for(int i = 1; i < argc; i++){
        struct stat fInfo;
        check_error(stat(argv[i], &fInfo) != -1, "Nepostojeci fajl");
        check_error(S_ISDIR(fInfo.st_mode), "Nije dir");

        // S_IRWXO predstavlja disjunkciju r, w, a za ostale korisnike
        if(!(fInfo.st_mode & S_IRWXO)){
            char* absPath = realpath(argv[i], NULL);
            check_error(absPath != NULL, "realpath");
            printf("%d\n", (int)strlen(absPath));
            free(absPath);
        }
    }

    exit(EXIT_SUCCESS);
}
