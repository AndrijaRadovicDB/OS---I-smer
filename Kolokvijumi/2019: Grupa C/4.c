// Program pravi fajl na trecoj putanji sa pravima pristupa koja imaju oba fajla
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

    check_error(argc == 4, "Argumenti");

    struct stat fInfo1;
    check_error(stat(argv[1], &fInfo1) != -1, "Nepostojeci fajl");
    struct stat fInfo2;
    check_error(stat(argv[2], &fInfo2) != -1, "Nepostojeci fajl");

    mode_t prava_fajla = 0;

    int fd = open(argv[3], O_CREAT);
    check_error(fd != -1, "open");

    if((fInfo1.st_mode & S_IRUSR) && (fInfo2.st_mode & S_IRUSR))
        prava_fajla |= S_IRUSR;
    if((fInfo1.st_mode & S_IWUSR) && (fInfo2.st_mode & S_IWUSR))
        prava_fajla |= S_IWUSR;
    if((fInfo1.st_mode & S_IXUSR) && (fInfo2.st_mode & S_IXUSR))
        prava_fajla |= S_IXUSR;
    if((fInfo1.st_mode & S_IRGRP) && (fInfo2.st_mode & S_IRGRP))
        prava_fajla |= S_IRGRP;
    if((fInfo1.st_mode & S_IWGRP) && (fInfo2.st_mode & S_IWGRP))
        prava_fajla |= S_IWGRP;
    if((fInfo1.st_mode & S_IXGRP) && (fInfo2.st_mode & S_IXGRP))
        prava_fajla |= S_IXGRP;
    if((fInfo1.st_mode & S_IROTH) && (fInfo2.st_mode & S_IROTH))
        prava_fajla |= S_IROTH;
    if((fInfo1.st_mode & S_IWOTH) && (fInfo2.st_mode & S_IWOTH))
        prava_fajla |= S_IWOTH;
    if((fInfo1.st_mode & S_IXOTH) && (fInfo2.st_mode & S_IXOTH))
        prava_fajla |= S_IXOTH;

    check_error(fchmod(fd, prava_fajla) != -1, "fchmod");

    close(fd);

    exit(EXIT_SUCCESS);
}
