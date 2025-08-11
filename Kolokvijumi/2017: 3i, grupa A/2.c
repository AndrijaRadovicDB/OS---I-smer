// Program za datu putanju ispisuje korisnicko ime vlasnika, naziv vlasnicke grupe
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <pwd.h>
#include <grp.h>

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

#define MAX_SIZE 1024

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "Nepostojeci fajl");

    struct passwd* userInfo = getpwuid(fInfo.st_uid);
    check_error(userInfo != NULL, "getpwuid");
    char* userName = malloc(strlen(userInfo->pw_name) + 1);
    check_error(userName != NULL, "malloc");
    strcpy(userName, userInfo->pw_name);

    struct group* groupInfo = getgrgid(fInfo.st_gid);
    check_error(groupInfo != NULL, "getgrgid");
    char* groupName = malloc(strlen(groupInfo->gr_name) + 1);
    check_error(groupName != NULL, "malloc");
    strcpy(groupName, groupInfo->gr_name);

    printf("%s %s\n", userName, groupName);

    free(userName);
    free(groupName);

    exit(EXIT_SUCCESS);
}
