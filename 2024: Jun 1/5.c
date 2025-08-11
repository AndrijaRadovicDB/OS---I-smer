// Program cita fajl rec po rec i ako se rec zavrsava sa _test pokusava da je
// zakljuca za pisanje. Ako rec ne moze da se zakljuca nastavlja se dalje.
// Ako je rec uspesno zakljucana program zamenjuje njen svaki karakter sa #.
// Maksimalna duzina reci je 256
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
        char* tmp = strrchr(word, '_');
        if(tmp != NULL && !strcmp(tmp, "_test")){
            struct flock fLock;
            fLock.l_type = F_WRLCK;
            fLock.l_whence = SEEK_SET;
            fLock.l_start = ftell(f);
            fLock.l_len = -strlen(word);

            int ret_val = fcntl(fd, F_SETLK, &fLock);
            if(ret_val == -1){
                if(errno != EACCES && errno != EAGAIN){
                    check_error(0, "errno");
                }
            }
            else{
                for(int i = 0; word[i]; i++){
                    word[i] = '#';
                }
                check_error(fseek(f, -strlen(word), SEEK_CUR) != -1, "fseek");
                fprintf(f, "%s", word);
                fLock.l_type = F_UNLCK;
                check_error(fcntl(fd, F_SETLK, &fLock) != -1, "fcntl");
            }
        }
    }
    check_error(feof(f), "feof");

    fclose(f);

    exit(EXIT_SUCCESS);
}
