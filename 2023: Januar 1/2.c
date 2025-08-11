// Program rekurzivno obilazi direktorijum sa nftw i racuna ukupnu veliciu svih
// regularnih fajlova sa trazenom ekstenzijom koji su modifikovani u poslednjih
// 7 dana i nju ispisuje.
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
        if(tmp != NULL && !strncmp(tmp, ext, strlen(ext))){
            time_t now = time(NULL);
            // Vreme bi trebalo da je dato u sekundama od epohe, ako nije
            // onda bi uradili ovo zakomentarisano
            //struct tm* broken_time = localtime(&sb->st_mtime);
            //time_t mtime = mktime(broken_time);
            time_t diff = (now - sb->st_mtime)/(24*3600);
            if(diff <= 7){
                size += sb->st_size;
            }
        }
    }

    return 0;
}


int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "stat");
    check_error(S_ISDIR(fInfo.st_mode), "Nije dir");
    ext = argv[2];

    check_error(nftw(argv[1], fn, 50, 0) != -1, "nftw");
    printf("%d\n", size);

    exit(EXIT_SUCCESS);
}
