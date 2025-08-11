// Program ispisuje da ako simbolicki link i fajl na koji pokazuje imaju istog
// vlasnika i vlasnicku grupu, inace ne
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

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

    struct stat fInfo1;
    check_error(lstat(argv[1], &fInfo1) != -1, "Nepostojeci fajl");
    check_error(S_ISLNK(fInfo1.st_mode), "Nije simbolicki link");

    uid_t user1 = fInfo1.st_uid;
    gid_t group1 = fInfo1.st_gid;

    struct stat fInfo2;
    check_error(stat(argv[1], &fInfo2) != -1, "Nepostojeci fajl");
    check_error(S_ISREG(fInfo2.st_mode), "Nije regularni fajl");


    uid_t user2 = fInfo2.st_uid;
    gid_t group2 = fInfo2.st_gid;

    if(user1 == user2 && group1 == group2){
        printf("da\n");
    }
    else{
        printf("ne\n");
    }

    exit(EXIT_SUCCESS);
}
