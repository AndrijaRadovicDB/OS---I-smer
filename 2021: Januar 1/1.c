// Program menja prava pristupa za fajl, v -> g, g -> o, o -> v.
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

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "stat");

    mode_t owner = fInfo.st_mode & (S_IRUSR | S_IWUSR | S_IXUSR);
    mode_t group = fInfo.st_mode & (S_IRGRP | S_IWGRP | S_IXGRP);
    mode_t others = fInfo.st_mode & (S_IROTH | S_IWOTH | S_IXOTH);
    // Ovaj umask uzima deafult vrednosti - koristimo ga kada treba da kreiramo
    // fajl, tako da u ovom zadatku nema smisla koristiti ga, jer ako ne postoji
    // fajl dobijamo gresku.
    // umask koristimo kako bi zastitli fajl od korisnika pri njegovoj kreaciji

    mode_t new_mode = (owner >> 6) | (group << 3) | (others << 3);
    check_error(chmod(argv[1], new_mode) != -1, "chmod");

    exit(EXIT_SUCCESS);
}
