// Program rekurzivno menja sve ekstenzije reg fajlova sa drugom ekstenzijom
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

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

char* ext_old = NULL;
char* ext_new = NULL;

void change_ext(char* fPath){

    struct stat fInfo;
    check_error(lstat(fPath, &fInfo) != -1, "lstat");

    if(S_ISREG(fInfo.st_mode)){
        char* tmp = strrchr(fPath, '.');
        if(tmp != NULL){
            if(!strcmp(tmp, ext_old)){
                size_t new_size = strlen(fPath) - strlen(ext_old) + strlen(ext_new);
                char* new_name = malloc(new_size);
                check_error(new_name != NULL, "malloc");

                int len = strlen(fPath) - strlen(ext_old);
                char* new_fPath = malloc(len);
                check_error(new_fPath != NULL, "malloc");
                for(int i = 0; i < len; i++){
                    new_fPath[i] = fPath[i];
                }
                new_fPath[len] = '\0';

                sprintf(new_name, "%s%s", new_fPath, ext_new);
                check_error(rename(fPath, new_name) != -1, "rename");

                free(new_name);
                free(new_fPath);
            }
        }
    }
    else if(S_ISDIR(fInfo.st_mode)){
        DIR* dir = opendir(fPath);
        check_error(dir != NULL, "opendir");
        check_error(chdir(fPath) != -1, "chdir");

        struct dirent* dirEntry;
        while((dirEntry = readdir(dir)) != NULL){
            if(!strcmp(dirEntry->d_name, ".") || !strcmp(dirEntry->d_name, "..")){
                continue;
            }
            change_ext(dirEntry->d_name);
        }

        check_error(chdir("..") != -1, "chdir");
        check_error(closedir(dir) != -1, "closedir");
    }

}

int main(int argc, char** argv){

    check_error(argc == 4, "Argumenti");

    ext_old = argv[2];
    ext_new = argv[3];

    struct stat fInfo;
    check_error(lstat(argv[1], &fInfo) != -1, "lstat");
    check_error(S_ISDIR(fInfo.st_mode), "Nije dir");

    change_ext(argv[1]);

    exit(EXIT_SUCCESS);
}
