// Program ipisuje sve linije fajla ciji ni jedan karakter nije zakljucan.
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

    check_error(argc == 2, "Argumenti");

    int fd = open(argv[1], O_RDONLY);
    check_error(fd != -1, "open");
    FILE* f = fdopen(fd, "r");
    check_error(f != NULL, "fdopen");

    ssize_t read_bytes = 0;
    size_t read = 0;
    char* line = NULL;
    while((read_bytes = getline(&line, &read, f)) != -1){
        struct flock fLock;
        fLock.l_type = F_WRLCK;
        fLock.l_whence = SEEK_SET;
        fLock.l_start = ftell(f);
        fLock.l_len = read; // Ne ide - jer ne citamo od pozadi

        check_error(fcntl(fd, F_GETLK, &fLock) != -1, "fcntl");

        if(fLock.l_type == F_UNLCK){
            printf("%s", line);
        }
    }

    free(line);
    fclose(f);

    exit(EXIT_SUCCESS);
}
