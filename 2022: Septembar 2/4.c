// Program cita rec po rec u fajlu i ispisuje ukupan broj zakljucanih karaktera.
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

    check_error(argc == 2, "Argumenti");

    int fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "open");

    int locked_chars = 0, offset = 0;
    char c;
    // Ako bi se uvek cela rec zakljucavala klasicno bi radili sa ftell itd, i
    // povecavali za velicinu reci
    while(read(fd, &c, 1) > 0){
        struct flock fLock;
        fLock.l_type = F_WRLCK;
        fLock.l_whence = SEEK_SET;
        fLock.l_start = offset;
        fLock.l_len = 1;

        check_error(fcntl(fd, F_GETLK, &fLock) != -1, "fcntl");
        if(fLock.l_type != F_UNLCK){
            locked_chars++;
        }
        offset++;
    }
    printf("%d\n", locked_chars);

    close(fd);

    exit(EXIT_SUCCESS);
}
