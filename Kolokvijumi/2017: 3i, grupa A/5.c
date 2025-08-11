// Program ispisuje ukupan broj fajlova u prosledjenom direktorijumu rekurzivno
// takodje treba da izracuna i broj fajlova svakog od postojecih tipova
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <ftw.h>

#include <stdio.h>
#include <stdlib.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

int niz_fajlova[8] = {0};

int countFiles(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf){

    if((sb->st_mode & S_IFMT) == S_IFREG){
        niz_fajlova[0]++;
    }
    if((sb->st_mode & S_IFMT) == S_IFDIR){
        niz_fajlova[1]++;
    }
    if((sb->st_mode & S_IFMT) == S_IFCHR){
        niz_fajlova[2]++;
    }
    if((sb->st_mode & S_IFMT) == S_IFBLK){
        niz_fajlova[3]++;
    }
    if((sb->st_mode & S_IFMT) == S_IFLNK){
        niz_fajlova[4]++;
    }
    if((sb->st_mode & S_IFMT) == S_IFIFO){
        niz_fajlova[5]++;
    }
    if((sb->st_mode & S_IFMT) == S_IFSOCK){
        niz_fajlova[6]++;
    }
    niz_fajlova[7]++;

    return 0;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "Nepostojeci fajl");
    check_error(S_ISDIR(fInfo.st_mode), "Nije direktorijum");

    check_error(nftw(argv[1], countFiles, 50, FTW_PHYS) != -1, "nftw");

    for(int i = 0; i < 8; i++){
        printf("%d ", niz_fajlova[i]);
    }
    printf("\n");

    exit(EXIT_SUCCESS);
}
