// Program cita fajl liniju po liniju. Ako linija nije zakljucana program treba
// da je upise u drugi prosledjen fajl.
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

    check_error(argc == 3, "Argumenti");

    int fd1 = open(argv[1], O_RDWR);
    check_error(fd1 != -1, "open");
    FILE* f1 = fdopen(fd1, "r+");
    check_error(f1 != NULL, "fdopen");
    FILE* f2 = fopen(argv[2], "w");
    check_error(f2 != NULL, "fopen");

    char* line = NULL;
    size_t len = 0;
    int written = 0;
    while(getline(&line, &len, f1) > 0){
        struct flock fLock;
        fLock.l_type = F_WRLCK;
        fLock.l_whence = SEEK_SET;
        fLock.l_start = ftell(f1);
        fLock.l_len = -strlen(line);

        check_error(fcntl(fd1, F_GETLK, &fLock) != -1, "fcntl");
        if(fLock.l_type == F_UNLCK){
            fprintf(f2, "%s", line);
            written++;
        }
    }
    check_error(feof(f1), "feof");

    printf("%d\n", written);

    free(line);
    fclose(f1);
    fclose(f2);

    exit(EXIT_SUCCESS);
}
