// Program rekurzivno obilazi direktorijum i ispisuje sve fifo fajlove u njemu
// cija grupa ima pravo citanja i pisanja.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <ftw.h>
#include <pwd.h>
#include <dirent.h>

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

int fn(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf){

    if(S_ISFIFO(sb->st_mode)){
        if((sb->st_mode & S_IWGRP) && (sb->st_mode & S_IRGRP)){
            char* tmp = realpath(fpath, NULL);
            check_error(tmp != NULL, "realpath");
            struct passwd* user = getpwuid(sb->st_uid);
            check_error(user != NULL, "getpwuid");
            printf("%s: %s\n", tmp, user->pw_name);
            free(tmp);
        }
    }

    return 0;
}

// Trebalo bi da radi
void proccess_fifo(char* fPath){

    DIR* dir = opendir(fPath);
    check_error(dir != NULL, "opendir");
    check_error(chdir(fPath) != -1, "chdir");

    struct dirent* dir_entry = NULL;
    while((dir_entry = readdir(dir)) != NULL){

        if(!strcmp(dir_entry->d_name, ".") || !strcmp(dir_entry->d_name, "..")){
            continue;
        }

        struct stat dInfo;
        check_error(stat(dir_entry->d_name, &dInfo) != -1, "stat");
        if(S_ISFIFO(dInfo.st_mode)){
            char* tmp = realpath(dir_entry->d_name, NULL);
            check_error(tmp != NULL, "realpath");
            struct passwd* user = getpwuid(dInfo.st_uid);
            check_error(user != NULL, "getpwuid");
            printf("%s: %s\n", tmp, user->pw_name);
            free(tmp);
        }
        else if(S_ISDIR(dInfo.st_mode)){
            if(strcmp(dir_entry->d_name, ".") && strcmp(dir_entry->d_name, "..")){
                proccess_fifo(dir_entry->d_name);
            }
        }

    }

    check_error(chdir("..") != -1, "chdir");
    check_error(closedir(dir) != -1, "closedir");
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "stat");
    check_error(S_ISDIR(fInfo.st_mode), "Nije dir");
    check_error(nftw(argv[1], fn, 50, 0) != -1, "nftw");
    //proccess_fifo(argv[1]);

    exit(EXIT_SUCCESS);
}
