// Program ipsisuje broj regularnih fajlova sa zadatom ekstenzijom
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <dirent.h>
#include <errno.h>

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

char* ext = NULL;

void numOfReg(char* putanja, unsigned* br_reg){

    struct stat fInfo;
    check_error(lstat(putanja, &fInfo) != -1, "Nepostojeci fajl");

    if(!S_ISDIR(fInfo.st_mode)){
        return;
    }

    DIR* dir = opendir(putanja);
    check_error(dir != NULL, "opendir");
    check_error(chdir(putanja) != -1, "chdir");

    struct dirent* dirEntry = NULL;
    errno = 0;
    while((dirEntry = readdir(dir)) != NULL){

        struct stat dirInfo;
        check_error(lstat(dirEntry->d_name, &dirInfo) != -1, "Nepostojeci fajl");

        if (!strcmp(dirEntry->d_name, ".") || !strcmp(dirEntry->d_name, "..")) {
            check_error(lstat(dirEntry->d_name, &fInfo) != -1, "...");
            errno = 0;
            continue;
        }

        if(S_ISREG(dirInfo.st_mode)){
            char* fExt = strrchr(dirEntry->d_name, '.');
            if(fExt != NULL && !strcmp(fExt, ext)){
                (*br_reg)++;
            }
        }

        numOfReg(dirEntry->d_name, br_reg);
        errno = 0;
    }
    check_error(errno != EBADF, "Los deskriptor");
    check_error(chdir("..") != -1, "chdir");
    check_error(closedir(dir) != -1, "closedir");

}

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "Nepostojeci fajl");
    check_error(S_ISDIR(fInfo.st_mode), "Nije dir");

    ext = argv[2];
    unsigned br_reg = 0;
    numOfReg(argv[1], &br_reg);
    printf("%u\n", br_reg);

    exit(EXIT_SUCCESS);
}
