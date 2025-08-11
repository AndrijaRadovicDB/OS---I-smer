// Program ispisuje sve poddirektorijume direktorijuma na dubini od 1 do 3
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <ftw.h>

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

int osDirDepth(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf){

    if(typeflag == FTW_D){
        if(ftwbuf->level > 0 && ftwbuf->level < 4){
            char* niska = malloc(sizeof(char)*MAX_SIZE);
            check_error(niska != NULL, "malloc");

            char* postoji = strrchr(fpath, '/');
            check_error(postoji != NULL, "strrchr");
            size_t duzina = strlen(postoji);

            niska = strcpy(niska, postoji + 1);
            niska[duzina] = '\0';
            if(strrchr(niska, '.') != NULL){
                printf("%s\n", niska);
            }

            free(niska);
        }
    }

    return 0;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "Nepostojeci fajl");
    check_error(S_ISDIR(fInfo.st_mode), "Nije dir");

    check_error(nftw(argv[1], osDirDepth, 50, FTW_PHYS) != -1, "nftw");

    exit(EXIT_SUCCESS);
}
