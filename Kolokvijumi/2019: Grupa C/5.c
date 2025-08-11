// Program ispisuje sve regularne fajlove koji su modifikovani pre 5 dana
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

void printDir(char* fpath){

    struct stat fInfo;
    check_error(lstat(fpath, &fInfo) != -1, "Nepostojeci fajl");

    DIR* dir_stream = opendir(fpath);
    check_error(dir_stream != NULL, "opendir");
    check_error(chdir(fpath) != -1, "chdir");

    struct dirent* dir_entry = NULL;
    errno = 0;
    while((dir_entry = readdir(dir_stream)) != NULL){
        struct stat dirInfo;
        check_error(lstat(dir_entry->d_name, &dirInfo) != -1, "Nepostojeci fajl");

        if(!strcmp(dir_entry->d_name, ".") || !strcmp(dir_entry->d_name, "..")){
            errno = 0;
            continue;
        }

        if(S_ISREG(dirInfo.st_mode)){
            time_t now = time(NULL);
            time_t razlika = (long int)(now - dirInfo.st_mtime + 24*60*60 - 1)/(24*60*60);
            if(razlika <= 5){
                printf("%s\n", dir_entry->d_name);
            }
        }
        else if(S_ISDIR(dirInfo.st_mode)){
            printDir(dir_entry->d_name);
        }
        errno = 0;
    }
    check_error(errno != EBADF, "Los fajl deskriptor");
    check_error(chdir("..") != -1, "chdir");
    check_error(closedir(dir_stream) != -1, "closedir");

}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    struct stat fInfo;
    check_error(lstat(argv[1], &fInfo) != -1, "Nepostojeci fajl");
    check_error(S_ISDIR(fInfo.st_mode), "Nije dir");

    printDir(argv[1]);

    exit(EXIT_SUCCESS);
}
