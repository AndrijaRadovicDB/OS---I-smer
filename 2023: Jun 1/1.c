// Program rucno rekurzivno obilazi direktorijum i ispisuje aposlutne putanje do
// svih git repozitorijuma u tom direktorijumi (dir koji sadrzi .git dir)
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

void proccess_dir(char* fPath){

    DIR* dir = opendir(fPath);
    check_error(dir != NULL, "opendir");
    check_error(chdir(fPath) != -1, "chdir");

    struct dirent* dir_entry = NULL;
    while((dir_entry = readdir(dir)) != NULL){

        if(!strcmp(dir_entry->d_name, ".") || !strcmp(dir_entry->d_name, "..")){
            continue;
        }

        struct stat dInfo;
        check_error(lstat(dir_entry->d_name, &dInfo) != -1, "lstat");

        if(S_ISDIR(dInfo.st_mode)){
            if(strstr(dir_entry->d_name, ".git") != NULL){
                char* tmp = realpath(dir_entry->d_name, NULL);
                check_error(tmp != NULL, "realpath");

                char* str = malloc((strlen(tmp) + 1)*sizeof(char));
                check_error(str != NULL, "malloc");
                for(int i = 0; tmp[i]; i++){
                    str[i] = tmp[i];
                }
                str[strlen(tmp)] = '\0';

                int len = strlen(str) - 5;
                char* final = malloc(len*sizeof(char));
                check_error(final != NULL, "malloc");
                strncpy(final, str, len);
                final[strlen(str) - 5] = '\0';
                printf("%s\n", final);

                free(tmp);
                free(str);
                free(final);
            }
            else{
                proccess_dir(dir_entry->d_name);
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

    proccess_dir(argv[1]);

    exit(EXIT_SUCCESS);
}
