// Program na datoj poziciji menja velika slova u mala i obrnuto
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

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
    check_error(S_ISREG(fInfo.st_mode), "Nije regularni fajl");

    unsigned pocetak = atoi(argv[2]);
    unsigned duzina = atoi(argv[3]);

    char* buffer = malloc(sizeof(char)*pocetak*4);
    check_error(buffer != NULL, "malloc");
    int readbytes;
    char* niska = malloc(sizeof(char)*duzina + 1);
    check_error(niska != NULL, "malloc");

    int fd = open(argv[1], O_RDONLY);
    check_error(fd != -1, "open");

    readbytes = read(fd, buffer, pocetak + duzina);
    check_error(readbytes != -1, "read");

    for(int i = 0; i < duzina; i++){
        if(islower(buffer[pocetak + i])){
            niska[i] = toupper(buffer[pocetak + i]);
        }
        else if(isupper(buffer[pocetak + i])){
            niska[i] = tolower(buffer[pocetak + i]);
        }
        else{
            niska[i] = buffer[pocetak + i];
        }
    }
    niska[duzina] = '\0';

    close(fd);

    fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "open");

    readbytes = read(fd, buffer, pocetak);
    check_error(readbytes != -1, "read");

    int writebytes = write(fd, niska, duzina);
    check_error(writebytes != -1, "write");

    close(fd);

    free(buffer);
    free(niska);

    exit(EXIT_SUCCESS);
}
