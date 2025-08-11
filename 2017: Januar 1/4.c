// Program gleda da je deo fajla na [a, a + b] zakljucana, ako nije, ispisuje
// unlocked, ako je zakljucano da moze da se cita, ali ne da se pise shared lock,
// a ako je zakljucan za oba pisati exclusive lock
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>

#include <stdio.h>
#include <stdlib.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

int main(int argc, char** argv){

    check_error(argc == 4, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "Nepostojeci fajl");

    int a = atoi(argv[2]);
    int b = atoi(argv[3]);

    int fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "open");

    // Moramo navesti svoja katanca koji postavljamo
    struct flock fLock;
    fLock.l_type = F_WRLCK;
    fLock.l_whence = SEEK_SET; // U odnosu na sta pocinje
    fLock.l_start = a;
    fLock.l_len = b;

    // F_GETLK - proverava da li moze da se postavi zeljeni katanac
    check_error(fcntl(fd, F_GETLK, &fLock) != -1, "fcntl"); // Dobijamo podatke
    switch(fLock.l_type){
        case F_UNLCK:
            printf("unlocked\n");
            break;
        case F_RDLCK:
            printf("shared lock\n");
            break;
        case F_WRLCK:
            printf("exclusive lock\n");
            break;
        default:
            break;
    }

    close(fd);

    exit(EXIT_SUCCESS);
}
