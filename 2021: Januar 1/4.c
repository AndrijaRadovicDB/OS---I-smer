// Program rekurzivno obilazi direktorijum sa nftw. Ispisuje broj fajlova na
// dubini od p do k sa zadatom ekstenzijom.
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

int p, k;
int ext_num = 0;
char* ext = NULL;

int dir_depth(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf){

    if(S_ISREG(sb->st_mode)){
        if(ftwbuf->level >= p && ftwbuf->level <= k){
            char* tmp = strrchr(fpath, '.');
            if(tmp != NULL && !strcmp(tmp + 1, ext)){
                ext_num++;
            }
        }
    }

    return 0;
}

int main(int argc, char** argv){

    check_error(argc == 5, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "stat");
    check_error(S_ISDIR(fInfo.st_mode), "Nije direktorijum");

    ext = argv[2];
    p = atoi(argv[3]);
    k = atoi(argv[4]);

    check_error(nftw(argv[1], dir_depth, 50, 0) != -1, "nftw");
    printf("%d\n", ext_num);

    exit(EXIT_SUCCESS);
}
