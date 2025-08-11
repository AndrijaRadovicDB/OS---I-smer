// Program ispisuje da ako je ime vlasnicke grupe jednako nazivu vlasnika fajla, inace ne
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <pwd.h>
#include <grp.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

bool ista_imena(const char* fpath){

    struct stat fInfo;
    check_error(stat(fpath, &fInfo) != -1, "Nepostojeci fajl");

    struct passwd* user = getpwuid(fInfo.st_gid);
    check_error(user != NULL, "getpwuid");

    struct group* grupa = getgrgid(fInfo.st_gid);
    check_error(grupa != NULL, "getgrgid");

    return !strcmp(user->pw_name, grupa->gr_name);
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    if(ista_imena(argv[1])){
        printf("da\n");
    }
    else{
        printf("ne\n");
    }

    exit(EXIT_SUCCESS);
}
