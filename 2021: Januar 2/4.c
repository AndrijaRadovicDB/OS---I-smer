// Program ispisuje broj fajlova takvih da je string s podstring u nazivu
// roditeljskog direktorijuma. Obavezno je rucno pisanje obilaska.
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

int name_num = 0;

void process_dir(const char* fPath, char* substring){

    struct stat fInfo;
    check_error(stat(fPath, &fInfo) != -1, "stat");

    if(!S_ISDIR(fInfo.st_mode)){
        return;
    }

    DIR* dir = opendir(fPath);
    check_error(dir != NULL, "opendir");

    struct dirent* dir_entry = NULL;
    while((dir_entry = readdir(dir)) != NULL){

        if(!strcmp(dir_entry->d_name, ".") || !strcmp(dir_entry->d_name, "..")){
            continue;
        }

        if(strstr(fPath, substring) != NULL){
            char* tmp = strrchr(fPath, '/');
            if(strstr(tmp, substring) != NULL){
                name_num++;
            }
        }

        // Ovo se radi jer mi zapravo nismo promenili ime koje obilazimo,
        // a da koristimo ime direktorijuma ne bi smo dobili celu putanju, vec
        // bukvalno samo ime fajla
        // printf("%s\n", dir_entry->d_name);
        char* name = malloc((strlen(fPath) + strlen(dir_entry->d_name) + 2)*sizeof(char));
        check_error(name != NULL, "malloc");
        strcpy(name, fPath);
        strcat(name, "/");
        strcat(name, dir_entry->d_name);

        process_dir(name, substring);
        free(name);
    }

    check_error(closedir(dir) != -1, "closedir");

}

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "stat");
    check_error(S_ISDIR(fInfo.st_mode), "Nije dir");

    process_dir(argv[1], argv[2]);
    printf("%d\n", name_num);

    exit(EXIT_SUCCESS);
}
