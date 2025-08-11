// Program premesta fajl sa srcPath na destPath. Fajl ne treba da promeni prava
// pristupa i zabranjeno je korisiti rename().
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

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

#define MAX_SIZE 1024

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    // realpath prosiruje sve simbolicke linkove i razresava sve reference
    char* srcPath = realpath(argv[1], NULL);
    check_error(srcPath != NULL, "realpath");
    char* destPath = realpath(argv[2], NULL);
    if(srcPath != NULL && destPath != NULL){
        check_error(strcmp(srcPath, destPath) != 0, "src i dest su isti");
    }
    free(srcPath);
    free(destPath);

    struct stat srcInfo;
    check_error(stat(argv[1], &srcInfo) != -1, "stat");
    int fdSrc = open(argv[1], O_RDONLY);
    check_error(fdSrc != -1, "open");

    mode_t old_umask = umask(0);

    // Ako je fajl otvoren u O_RDWR ili O_WRONLY, O_TRUNC brise sadrzaj fajla
    int fdDest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, srcInfo.st_mode);
    check_error(fdDest != -1, "open");

    umask(old_umask);


    // Premestanje fajlova se svodi na citanje sa jednog mesta i pisanja na drugo
    char buffer[MAX_SIZE];
    int read_bytes = 0;
    while((read_bytes = read(fdSrc, buffer, MAX_SIZE)) > 0){
        check_error(write(fdDest, buffer, read_bytes) != -1, "write");
    }
    check_error(read_bytes != -1, "read");

    check_error(unlink(argv[1]) != -1, "unlink");

    close(fdSrc);
    close(fdDest);

    exit(EXIT_SUCCESS);
}
