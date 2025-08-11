// Program rekurzivno obilazi direktorijum i izracunava velicinu svih regularnih
// fajlova sa trazenom ekstenzijom koji su modifikovani u poslednjih 7 dana.
// Koristiti nftw.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <ftw.h>
#include <time.h>

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
unsigned size = 0;

int fn(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf){

    if(typeflag == FTW_F){
        char* tmp = strrchr(fpath, '.');
        if(tmp != NULL && !strcmp(tmp, ext)){
            time_t now = time(NULL);
            time_t mtime_days = (now - sb->st_mtime)/(24*3600);
            if(mtime_days <= 7){
                size += sb->st_size;
            }
        }
    }

    return 0;
}

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    ext = argv[2];
    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "stat");
    check_error(S_ISDIR(fInfo.st_mode), "Nije dir");
    check_error(nftw(argv[1], fn, 50, 0) != -1, "nftw");
    printf("%u\n", size);

    exit(EXIT_SUCCESS);
}
