// Program rucno rekurzijom obilazi direktorijum i ispisuje naziv simbolickog
// linka koji je poslednji modifikovan. Ako program ne naidje na simbolicki link
// treba ispisati neuspeh.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <dirent.h>
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

char* symlnk_name = NULL;
time_t max_mtime = 0;


void proccess_dir(char* fPath){

    DIR* dir = opendir(fPath);
    check_error(dir != NULL, "opendir");
    check_error(chdir(fPath) != -1, "chdir");

    struct dirent* dir_entry = NULL;
    while((dir_entry = readdir(dir)) != NULL){

        struct stat dInfo;
        check_error(lstat(dir_entry->d_name, &dInfo) != -1, "lstat");

        if(S_ISLNK(dInfo.st_mode)){
            if(dInfo.st_mtime > max_mtime){
                max_mtime = dInfo.st_mtime;
                if(max_mtime == 0){
                    symlnk_name = malloc((strlen(dir_entry->d_name) + 1)*sizeof(char));
                    check_error(symlnk_name != NULL, "malloc");
                }
                else{
                    symlnk_name = realloc(symlnk_name, (strlen(dir_entry->d_name) + 1)*sizeof(char));
                    check_error(symlnk_name != NULL, "realloc");
                }
                strcpy(symlnk_name, dir_entry->d_name);
            }
        }
        else if(S_ISDIR(dInfo.st_mode)){
            // Ako ne gledamo nas ili roditelja
            if(strcmp(dir_entry->d_name, ".") && strcmp(dir_entry->d_name, "..")){
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

    if(max_mtime == 0){
        printf("neuspeh\n");
    }
    else{
        printf("%s\n", symlnk_name);
        free(symlnk_name);
    }

    exit(EXIT_SUCCESS);
}
