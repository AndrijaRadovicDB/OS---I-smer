// Program za datu reci ispisuje poziciju i tip katanca za sva zakljucavanja.
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

#define MAX_SIZE 255

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    int fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "open");
    FILE* f = fdopen(fd, "r");
    check_error(f != NULL, "fdopen");
    char word[MAX_SIZE];
    strcpy(word, argv[2]);

    char tmp[MAX_SIZE];
    while(fscanf(f, "%s", tmp) == 1){
        if(!strcmp(tmp, word)){
            struct flock fLock;
            fLock.l_type = F_WRLCK;
            fLock.l_whence = SEEK_SET;
            fLock.l_start = ftell(f);
            fLock.l_len = -strlen(tmp);

            check_error(fcntl(fd, F_GETLK, &fLock) != -1, "fcntl");
            if(fLock.l_type == F_WRLCK){
                // Jer je l_start na kraju reci ne na pocetku
                printf("%ld w", ftell(f) - strlen(tmp));
            }
            else if(fLock.l_type = F_RDLCK){
                printf("%ld r", ftell(f) - strlen(tmp));
            }
        }
    }
    check_error(feof(f), "feof");

    fclose(f);

    exit(EXIT_SUCCESS);
}
