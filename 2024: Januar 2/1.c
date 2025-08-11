// Program nalazi najvece vreme pristupa i najvece vreme modifikacije i pristupa
// medju fajlovima i postavlja svim fajlovima postavlja ta nova vremena. U obzir
// ne uzimamo poddirektorijume.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <dirent.h>
#include <time.h>
#include <utime.h>

#include <stdio.h>
#include <stdlib.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

time_t max_mtime = 0;
time_t max_atime = 0;

void max_a_m_time(const char* fPath){

    DIR* dir = opendir(fPath);
    check_error(dir != NULL, "opendir");

    struct dirent* dir_entry = NULL;
    while((dir_entry = readdir(dir)) != NULL){

        struct stat dInfo;
        check_error(stat(dir_entry->d_name, &dInfo) != -1, "stat");

        if(dInfo.st_mtime > max_mtime){
            max_mtime = dInfo.st_mtime;
        }
        if(dInfo.st_atime > max_atime){
            max_atime = dInfo.st_atime;
        }

    }

    check_error(closedir(dir) != -1, "closedir");

}

void change_a_m_time(const char* fPath){

    DIR* dir = opendir(fPath);
    check_error(dir != NULL, "opendir");

    struct utimbuf new_time;
    new_time.modtime = max_mtime;
    new_time.actime = max_atime;

    struct dirent* dir_entry = NULL;
    while((dir_entry = readdir(dir)) != NULL){

        struct stat dInfo;
        check_error(stat(dir_entry->d_name, &dInfo) != -1, "stat");

        check_error(utime(dir_entry->d_name, &new_time) != -1, "utime");

    }

    check_error(closedir(dir) != -1, "closedir");
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    max_a_m_time(".");
    change_a_m_time(".");

    exit(EXIT_SUCCESS);
}
