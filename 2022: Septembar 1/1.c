// Program rekurzivno obilazi direktorijum sa nftw, ako je regularan fajl
// ispisuje njegov naziv i dan u nedelji kada je poslednji put modifikovan
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

void print_day(int num){

    if(num == 0){
        printf("nedelja\n");
    }
    else if(num == 1){
        printf("ponedeljak\n");
    }
    else if(num == 2){
        printf("utorak\n");
    }
    else if(num == 3){
        printf("sreda\n");
    }
    else if(num == 4){
        printf("cetvrtak\n");
    }
    else if(num == 5){
        printf("petak\n");
    }
    else if(num == 6){
        printf("subota\n");
    }

}


int fn(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf){

    if(typeflag == FTW_F){
        time_t mtime = sb->st_mtime;
        struct tm* broken_time = localtime(&mtime);
        check_error(broken_time != NULL, "localtime");
        printf("%s ", fpath + ftwbuf->base);
        print_day(broken_time->tm_wday);
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
