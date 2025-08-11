// Program ispisuje poziciji i tip katanca za sva zakljucana poljavljivanja date
// reci u fajlu
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

#define MAX_SIZE 256

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    int fd = open(argv[1], O_RDONLY);
    check_error(fd != -1, "open");

    FILE* f = fdopen(fd, "r");
    check_error(f != NULL, "fdopen");

    int duzina_reci = strlen(argv[2]);
    char buffer[MAX_SIZE];

    while(!feof(f)){
        fscanf(f, "%s", buffer);

        if(!strcmp(argv[2], buffer)){
            struct flock fLock;
            fLock.l_type = F_WRLCK;
            fLock.l_whence = SEEK_SET;
            fLock.l_start = ftell(f);
            fLock.l_len = -duzina_reci;

            check_error(fcntl(fd, F_GETLK, &fLock) != -1, "fcntl");

            long trenutni_offset = ftell(f) - duzina_reci;
            switch(fLock.l_type){
                case F_WRLCK:
                    printf("%ld w\n", trenutni_offset);
                    break;
                case F_RDLCK:
                    printf("%ld r\n", trenutni_offset);
                    break;
                case F_UNLCK:
                    break;
            }
        }
    }

    fclose(f);

    exit(EXIT_SUCCESS);
}
