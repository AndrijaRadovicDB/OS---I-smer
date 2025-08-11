// Program cita fajl rec po rec i sve zakljucane reci se ispisuju kao i tip
// katanca, r za read, w za write. Otkljucane reci se preskacu.
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
            exit(EXIT_SUCCESS);\
        }\
    } while(0)

#define MAX_SIZE 256

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    int fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "open");
    FILE* f = fdopen(fd, "r+");
    check_error(f != NULL, "fdopen");

    char word[MAX_SIZE];
    while(fscanf(f, "%s", word) == 1){
        struct flock fLock;
        fLock.l_type = F_WRLCK;
        fLock.l_whence = SEEK_SET;
        fLock.l_start = ftell(f);
        fLock.l_len = -strlen(word);

        check_error(fcntl(fd, F_GETLK, &fLock) != -1, "fcntl");
        if(fLock.l_type == F_WRLCK){
            printf("%s w\n", word);
        }
        else if(fLock.l_type == F_RDLCK){
            printf("%s r\n", word);
        }
    }
    check_error(feof(f), "feof");

    fclose(f);

    exit(EXIT_SUCCESS);
}
