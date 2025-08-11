// Program ispisuje sadrzaj prosledjenog simbolickog linka
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

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
    check_error(lstat(argv[1], &fInfo) != -1, "Nepostojeci fajl");
    check_error(S_ISLNK(fInfo.st_mode), "Nije simbolicki link");

    char buffer[MAX_SIZE];
    int read_bytes = readlink(argv[1], buffer, MAX_SIZE);
    check_error(read_bytes != -1, "readlink");
    buffer[read_bytes] = '\0';
    printf("%s\n", buffer);

    exit(EXIT_SUCCESS);
}
