// Program cita fajl od pozicije a do pozicije b, sto je ime direktorijuma koji
// treba kreirati sa svim pravim pristupa i ispisati naziv kreiranog fajla.
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

int main(int argc, char** argv){

    check_error(argc == 4, "Argumenti");

    int fd = open(argv[1], O_RDONLY);
    check_error(fd != -1, "open");
    int a = atoi(argv[2]);
    int b = atoi(argv[3]);

    char* buffer = malloc(b*sizeof(char));
    check_error(buffer != NULL, "malloc");
    check_error(lseek(fd, a, SEEK_SET) != -1, "lseek");
    check_error(read(fd, buffer, b) != -1, "read");
    printf("%s\n", buffer);

    mode_t old_umask = umask(0);
    check_error(mkdir(buffer, 0777) != -1, "mkdir");
    umask(old_umask);

    close(fd);
    free(buffer);

    exit(EXIT_SUCCESS);
}
