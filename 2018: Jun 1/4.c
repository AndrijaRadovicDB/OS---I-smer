// Program prolazi kroz fajl i svaku otkljucanu rec obrce, u slucaju da je rec
// zakljucana nastavlja dalje. Na kraju ispisuje broj uspeno i neuspesno
// izmenjenih reci.
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

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    int fd = open(argv[1],  O_RDWR);
    check_error(fd != -1, "open");

    int num_success = 0, num_fail = 0;
    char c;
    char word[256];

/*  Ovo je bez pretpostavke da rec ima maks 256 slova
    int i = 0, offset = 0, wrd_start = 0, len = 0;
    while(read(fd, &c, 1) > 0){
        if(c == ' ' || c == '\n'){
            word[i] = '\0';

            struct flock fLock;
            fLock.l_type = F_WRLCK;
            fLock.l_whence = SEEK_SET;
            fLock.l_start = wrd_start;
            fLock.l_len = len;

            check_error(fcntl(fd, F_GETLK, &fLock) != -1, "fcntl");

            if(fLock.l_type == F_UNLCK){
                for(int i = 0; i < len/2; i++){
                    char tmp = word[i];
                    word[i] = word[len - i - 1];
                    word[len - i - 1] = tmp;
                }

                check_error(lseek(fd, wrd_start, SEEK_SET) != -1, "lseek");
                check_error(write(fd, word, len) != -1, "write");
                check_error(lseek(fd, 1, SEEK_CUR) != -1, "lseek");

                num_success++;
            }
            else{
                num_fail++;
            }

            wrd_start = offset + 1;
            len = 0;
            i = 0;
        }
        else{
            word[i] = c;
            i++;
            len++;
        }
        offset++;
    }*/

    FILE* f = fdopen(fd, "r+");
    check_error(f != NULL, "fdopen");

    while(fscanf(f, "%s", word) == 1){
        struct flock fLock;
        fLock.l_type = F_WRLCK;
        fLock.l_whence = SEEK_SET;
        fLock.l_start = ftell(f);
        fLock.l_len = -strlen(word);

        check_error(fcntl(fd, F_GETLK, &fLock) != -1, "fcntl");

        if(fLock.l_type == F_UNLCK){
            int len = (int)strlen(word);
            for(int i = 0; i < len/2; i++){
                char tmp = word[i];
                word[i] = word[len - i - 1];
                word[len - i - 1] = tmp;
            }

            check_error(fseek(f, -len, SEEK_CUR) != -1, "fseek");
            fprintf(f, "%s", word);

            num_success++;
        }
        else{
            num_fail++;
        }
    }
    check_error(feof(f), "feof");

    printf("%d %d\n", num_success, num_fail);

    fclose(f);

    exit(EXIT_SUCCESS);
}
