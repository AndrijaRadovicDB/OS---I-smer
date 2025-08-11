// Program rekurzivno obilazi direktorijum i ispisuje sve programe
// koji se nalaze  u dir koji pocinu sa dir_
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <libgen.h> // za basename
#include <dirent.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

bool sadrzi_rec(char* fpath){

    char* dirname = basename(fpath); // Pretvara putanju u direktorijum i fajlove
    char prefiks[5] = {dirname[0], dirname[1], dirname[2], dirname[3], '\0'};

    return strcmp(prefiks, "dir_") == 0;
}

void ispisiFajlove(char* fpath, bool sadrzi){

    DIR* dir = opendir(fpath);
    check_error(dir != NULL, "opendir");
    check_error(chdir(fpath) != -1 , "chdir");

    struct dirent* dirEntry = NULL;
    errno = 0;
    while ((dirEntry = readdir(dir)) != NULL) {

        struct stat dirInfo;
        check_error(lstat(dirEntry->d_name, &dirInfo) != -1, "Nepostojeci fajl");

        if(S_ISREG(dirInfo.st_mode)){
            if(sadrzi){
                printf("%s\n", dirEntry->d_name);
            }
        }
        else if(S_ISDIR(dirInfo.st_mode)){
            // Ako nismo u roditeljskom direktorijumu
            if(strcmp(dirEntry->d_name, ".") && strcmp(dirEntry->d_name, "..")){
                ispisiFajlove(dirEntry->d_name, sadrzi_rec(dirEntry->d_name) || sadrzi);
            }
        }
        errno = 0;
    }

    check_error(errno != EBADF, "...");
    check_error(chdir("..") != -1, "...");
    check_error(closedir(dir) != -1, "closedir");
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "Nepostojeci fajl");
    check_error(S_ISDIR(fInfo.st_mode), "Nije dir");

    ispisiFajlove(argv[1], sadrzi_rec(argv[1]));

    exit(EXIT_SUCCESS);
}
