// Program mrenja sva pojavljivanja 1. reci sa 2. reci, pre prepisivanje treba da
// zakljuca staru rec, ako je vec zakljucana nastavlja dalje bez zamene reci. Na
// kraju ispisati broj reci koje nisu zamenjene.
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
            exit(EXIT_SUCCESS);\
        }\
    } while(0)

#define MAX_SIZE 256

int main(int argc, char** argv){

    check_error(argc == 4, "Argumenti");

    char* old_word = argv[2];
    char* new_word = argv[3];

    int fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "open");
    FILE* f = fdopen(fd, "r+");
    check_error(f != NULL, "fdopen");

    char word[MAX_SIZE];
    int failed = 0;
    while(fscanf(f, "%s", word) == 1){
        if(!strcmp(word, old_word)){
            struct flock fLock;
            fLock.l_type = F_WRLCK;
            fLock.l_whence = SEEK_SET;
            fLock.l_start = ftell(f);
            fLock.l_len = -strlen(word);

            int ret_val = fcntl(fd, F_SETLK, &fLock)
            if(ret_val == -1){
                if(errno != EACCES && errno != EAGAIN){
                    check_error(0, "errno");
                }
                failed++;
            }
            else{
                check_error(fseek(f, -strlen(word), SEEK_CUR) != -1, "fseek");
                fprintf(f, "%s", new_word);
                fLock.l_type = F_UNLCK;
                check_error(fcntl(fd, F_SETLK, &fLock) != -1, "fcntl");
            }
        }
    }
    check_error(feof(f), "feof");

    printf("%d\n", failed);

    fclose(f);

    exit(EXIT_SUCCESS);
}
