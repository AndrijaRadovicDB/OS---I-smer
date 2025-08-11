// Program ipisuje razliku izmedju najveceg i najmanjeg regularnog fajla
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

void velicinaFajla(const char* fPath, unsigned long long* min, unsigned long long* max){

    struct stat fInfo;
    check_error(lstat(fPath, &fInfo) != -1, "Nepostojeci fajl");

    if(S_ISREG(fInfo.st_mode)){
        *min = fInfo.st_size < *min ? fInfo.st_size : *min;
        *max = *max < fInfo.st_size ? fInfo.st_size : *max;
    }

    if(S_ISDIR(fInfo.st_mode)){
        DIR* dir = opendir(fPath);
        check_error(dir != NULL, "opendir");
        check_error(chdir(fPath) != -1, "chdir");

        struct dirent* dirEntry = NULL;
        while((dirEntry = readdir(dir)) != NULL){
            if(!strcmp(dirEntry->d_name, ".") || !strcmp(dirEntry->d_name, "..")){
                continue;
            }
            velicinaFajla(dirEntry->d_name, min, max);
        }

        check_error(chdir("..") != -1, "chdir");
        check_error(closedir(dir) != -1, "closedir");
    }
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    unsigned long long min = INT_MAX, max = 0;

    struct stat fInfo;
    check_error(lstat(argv[1], &fInfo) != -1, "Nepostojeci fajl");
    check_error(S_ISDIR(fInfo.st_mode), "Nije dir");

    velicinaFajla(argv[1], &min, &max);
    printf("%llu\n", max - min);

    exit(EXIT_SUCCESS);
}
