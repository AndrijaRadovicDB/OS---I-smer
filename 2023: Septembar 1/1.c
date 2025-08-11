// Program rekurzivno obilazi direktorijum i ispisuje apsolutne putanje svih
// git repozitorijuma u tom direktorijumu
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <ftw.h>

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

int fn(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf){

    if(typeflag == FTW_D){
        if(strstr(fpath, ".git") != NULL){
            char* tmp = realpath(fpath, NULL);
            check_error(tmp != NULL, "realpath");

            char* str = malloc((strlen(tmp) + 1)*sizeof(char));
            check_error(str != NULL, "malloc");
            for(int i = 0; tmp[i]; i++){
                str[i] = tmp[i];
            }
            str[strlen(tmp)] = '\0';

            int len = strlen(str) - 4;
            char* final = malloc(len*sizeof(char));
            check_error(final != NULL, "malloc");
            strncpy(final, str, len - 1);
            final[len] = '\0';
            printf("%s\n", final);

            free(tmp);
            free(str);
            free(final);
        }
    }

    return 0;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "stat");
    check_error(S_ISDIR(fInfo.st_mode), "Nije dir");
    check_error(nftw(argv[1], fn, 50, 0) != -1, "nftw");

    exit(EXIT_SUCCESS);
}
