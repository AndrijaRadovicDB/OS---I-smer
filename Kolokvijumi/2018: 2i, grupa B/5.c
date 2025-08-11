// Program ipisuje sve regularne fajlove izmedju nivoa 2 i 5 sa datom ekstenzijom
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

char* ekstenzija = NULL;

int ekstenzije(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf){

    if(typeflag == FTW_F){
        if(ftwbuf->level > 1 && ftwbuf->level < 6){
            if(strstr(fpath, ekstenzija) != NULL){
                char* postoji = strrchr(fpath, '/');
                check_error(postoji != NULL, "strrchr");
                char* niska = malloc(sizeof(char)*MAX_SIZE + 1);
                check_error(niska != NULL, "malloc");
                int duzina = (int)strlen(postoji);

                for(int i = 0; i < duzina; i++){
                    niska[i] = postoji[i + 1];
                }
                niska[duzina] = '\0';
                printf("%s\n", niska);

                free(niska);
            }
        }
    }

    return 0;
}

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "Nepostojeci fajl");
    check_error(S_ISDIR(fInfo.st_mode), "Nije dir");
    check_error(argv[2][0] == '.', "Los argument");

    ekstenzija = argv[2];

    check_error(nftw(argv[1], ekstenzije, 50, 0) != -1, "nftw");

    exit(EXIT_SUCCESS);
}
