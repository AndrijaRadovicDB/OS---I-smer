// Program menja sva pojavljivanja 1. reci sa 2. reci, pre prepisivanja stare reci
// treba je zakljucati, ako ju je neko vec zakljucao nastavljamo dalje bez zamene,
// i na kraju treba ispisati broj reci koje nisu zamenjene
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

    check_error(argc == 4, "Argumenti");

    char* stara_rec = argv[2];
    char* nova_rec = argv[3];
    int br_nezakljucanih = 0;

    int fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "fd");

    FILE* f = fdopen(fd, "r+");
    check_error(f != NULL, "fdopen");

    char trenutna_rec[MAX_SIZE];
    while(fscanf(f, "%s", trenutna_rec) == 1){
        if(!strcmp(trenutna_rec, stara_rec)){
            struct flock fLock;
            fLock.l_type = F_WRLCK;
            fLock.l_whence = SEEK_SET;
            fLock.l_start = ftell(f);
            fLock.l_len = -strlen(trenutna_rec);

            int ret_value = fcntl(fd, F_SETLK, &fLock);
            if(ret_value == -1){
                if(errno != EACCES && errno != EAGAIN){ // Ovako se uvek radi
                    check_error(0, "errno");
                }
                else{
                    br_nezakljucanih++;
                    fLock.l_type = F_UNLCK;
                    check_error(fcntl(fd, F_SETLK, &fLock) != -1, "fcntl");
                }
            }
            else{
                // fseek gleda u fajl na koji pokazuje trenutno u odnosu na
                // trenutnu poziciju, a nju umanjujemo za duzinu trenutne reci
                // kako bi dosli na pociziju gde treba da upisemo novu rec
                check_error(fseek(f, -strlen(trenutna_rec), SEEK_CUR) != -1, "fseek");
                fprintf(f, "%s", nova_rec);
            }
        }
    }
    check_error(feof(f), "EOF");

    fclose(f);

    printf("%d\n", br_nezakljucanih);

    exit(EXIT_SUCCESS);
}
