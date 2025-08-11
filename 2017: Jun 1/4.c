// Program cita rec po rec i ukoliko procita broj zakljucava ga za citanje, ako
// ne moze da ga zakljuca program nastavlja dalje normalno, na kraju ispisati
// broj uspesno zakljucanih brojeva
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

#define MAX_WORD 256
#define MAX_SIZE 1024

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    int fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "open");

    FILE* f = fdopen(fd, "r+");
    check_error(f != NULL, "fdopen");

    char buffer[MAX_SIZE];
    int uspesno_zakljucano = 0;
    while(fscanf(f, "%s", buffer) == 1){
        char *stringError = NULL;
        int povratna_vrednost = strtol(buffer, &stringError, 10);
        if(buffer != NULL && *stringError == '\0'){ // Ovo znaci da je uspesno citanje
            // Ovo je sablon kada radimo sa zakljucavanjem i stdio
            struct flock fLock;
            fLock.l_type = F_RDLCK;
            fLock.l_whence = SEEK_SET;
            fLock.l_start = ftell(f);
            fLock.l_len = -strlen(buffer);
            povratna_vrednost = fcntl(fd, F_SETLK, &fLock);
            if(povratna_vrednost == -1){
                if(errno != EACCES && errno != EAGAIN){
                    check_error(0, "fcntl");
                }
            }
            else{
                uspesno_zakljucano++;
                fLock.l_type = F_UNLCK;
                check_error(fcntl(fd, F_SETLK, &fLock) != -1, "fcntl");
            }
        }
    }
    // Proverava da li smo na kraju fajla, ako nismo vraca 0
    check_error(feof(f), "feof");

    fclose(f);

    printf("%d\n", uspesno_zakljucano);

    exit(EXIT_SUCCESS);
}
