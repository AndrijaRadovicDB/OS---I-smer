// Program cita fajl rec po rec i ukoliko procita pozitivan cetvorocifren broj
// treba da se potrudi da ga zakljuca, ako ne uspe nastavlja dalje, ako uspe
// zamenjuje taj broj u fajlu sa ####
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

#define MAX_SIZE (256)

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
            fLock.l_len = -strlen(word);

            int ret_val = fcntl(fd, F_SETLK, &fLock);
            if(ret_val == -1){
                if(errno != EAGAIN && errno != EACCES){
                    check_error(0, "errno");
                }
            }
            else{
                check_error(fseek(f, -strlen(word), SEEK_CUR) != -1, "fseek");
                fprintf(f, "####");
            }
        }
    }
    check_error(feof(f), "feof");

    fclose(f);

    exit(EXIT_SUCCESS);
}
