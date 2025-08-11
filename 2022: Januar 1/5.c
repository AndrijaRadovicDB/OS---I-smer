// Program cita fajl rec po rec, ako procita broj koji nije zakljucan treba da ga
// zameni sa onoliko # koliko je on dugacak. Na kraju treba da ispise broj
// izmenjenih karaktera
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

#define MAX_SIZE 255

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    int fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "open");
    FILE* f = fdopen(fd, "r+");
    check_error(f != NULL, "fdopen");

    int changed_chars = 0;
    char word[MAX_SIZE];
    while(fscanf(f, "%s", word) == 1){
        char* endptr = NULL;
        int num = strtol(word, &endptr, 10);
        if(word[0] != '\0' && *endptr == '\0'){
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
            else{ // Rec bila otkljucana
                int n = strlen(word);
                changed_chars += n;
                char tmp[n + 1];
                for(int i = 0; i < n; i++){
                    tmp[i] = '#';
                }
                tmp[n] = '\0';
                check_error(fseek(f, -n, SEEK_CUR) != -1, "fseek");
                fprintf(f, "%s", tmp);

                fLock.l_type = F_UNLCK;
                check_error(fcntl(fd, F_SETLK, &fLock) != -1);
            }
        }
    }
    check_error(feof(f), "feof");

    printf("%d\n", changed_chars);

    fclose(f);

    exit(EXIT_SUCCESS);
}
