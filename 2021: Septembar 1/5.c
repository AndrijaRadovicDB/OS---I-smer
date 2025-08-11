// Program cita rec po rec i ako procita pozitivan 4cifren broj pokusava da ga
// zakljuca za pisanje. Ako ne uspe, nastavlja dalje normalno, a ako uspe umesto
// broja treba da pise ####
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>

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

    check_error(argc == 2, "Argumenti");

    int fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "open");
    FILE* f = fdopen(fd, "r+");
    check_error(f != NULL, "fdopen");

    char word[MAX_SIZE];
    while(fscanf(f, "%s", word) == 1){
        int num = atoi(word);
        if(num >= 1000 && num <= 9999){
            struct flock fLock;
            fLock.l_type = F_WRLCK;
            fLock.l_whence = SEEK_SET;
            fLock.l_start = ftell(f);
            fLock.l_len = -4;

            int ret_val = fcntl(fd, F_SETLK, &fLock);
            if(ret_val == -1){
                if(errno != EACCES && errno != EAGAIN){
                    check_error(0, "errno");
                }
            }
            else{
                check_error(fseek(f, -4, SEEK_CUR) != -1, "fseek");
                fprintf(f, "####");
                // Ako treba da se otkljuca
                // fLock.l_type = F_UNLCK;
                // check_error(fcntl(fd, &F_SETLK, &fLock) != -1, "fcntl");
            }
        }
    }
    check_error(feof(f), "feof");

    fclose(f);

    exit(EXIT_SUCCESS);
}
