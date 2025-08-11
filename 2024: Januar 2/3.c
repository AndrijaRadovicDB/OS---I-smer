// Program dobija putanju i ispisuje ime vlasnika i ime grupe fajla, ako je u
// pitanju simbolicki link, program treba da ispise informacije o linku, a ne o
// fajlu na koji link pokazuje.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <pwd.h>
#include <grp.h>

#include <stdio.h>
#include <stdlib.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    struct stat fInfo;
    check_error(lstat(argv[1], &fInfo) != -1, "lstat");

    struct passwd* usr = getpwuid(fInfo.st_uid);
    check_error(usr != NULL, "getpwuid");
    struct group* grp = getgrgid(fInfo.st_gid);
    check_error(grp != NULL, "getgrgid");
    printf("%s %s\n", usr->pw_name, grp->gr_name);

    exit(EXIT_SUCCESS);
}
