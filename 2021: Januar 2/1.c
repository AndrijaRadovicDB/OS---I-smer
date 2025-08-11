// Program menja prava pristupa drugog fajla sa prvim fajlom tako da v2 -> g1,
// g2 -> o1, o2 -> v1. Ako drugi fajl ne postoji ne treba ga praviti.
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

    check_error(argc == 3, "Argumenti");

    struct stat fInfo1;
    check_error(stat(argv[1], &fInfo1) != -1, "stat");
    struct stat fInfo2;
    check_error(stat(argv[2], &fInfo2) != -1, "stat");

    mode_t owner = fInfo1.st_mode & (S_IRUSR | S_IWUSR | S_IXUSR);
    mode_t group = fInfo1.st_mode & (S_IRGRP | S_IWGRP | S_IXGRP);
    mode_t others = fInfo1.st_mode & (S_IROTH | S_IWOTH | S_IXOTH);

    mode_t new_mode = (owner >> 6) | (group << 3) | (others << 3);
    check_error(chmod(argv[2], new_mode) != -1, "chmod");

    exit(EXIT_SUCCESS);
}
