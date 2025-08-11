// Program otvara fajl i trazi odredjenu rec, ako je zakljucana ispisati poziciju
// i tip katanca.
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

#define MAX_SIZE (255)

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    int fd = open(argv[1], O_RDONLY);
    check_error(fd != -1, "open");
    FILE* f = fdopen(fd, "r");
    check_error(f != NULL, "fdopen");

    char* word = argv[2];
    char tmp[MAX_SIZE];
    while(fscanf(f, "%s", tmp) == 1){
        if(!strcmp(word, tmp)){
            struct flock fLock;
            fLock.l_type = F_WRLCK;
            fLock.l_whence = SEEK_SET;
            fLock.l_start = ftell(f);
            fLock.l_len = -strlen(tmp);

            int ret_val = fcntl(fd, F_GETLK, &fLock);
            if(ret_val == -1){
                if(errno != EACCES && errno != EAGAIN){
                    check_error(0, "errno");
                }
            }
            else{
                // Oduzimamo od trenutne pozicije duzinu reci jer je trenutna
                // pozicija posle same reci, tako da je pozicija veca za njenu
                // duzinu
                if(fLock.l_type == F_WRLCK){
                    printf("%ld w\n", ftell(f) - strlen(tmp));
                }
                else if(fLock.l_type == F_RDLCK){
                    printf("%ld r\n", ftell(f) - strlen(tmp));
                }
            }
        }
    }
    check_error(feof(f), "feof");

    fclose(f);

    exit(EXIT_SUCCESS);
}
