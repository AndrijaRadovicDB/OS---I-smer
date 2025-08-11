// Program obrce tekst na datoj poziciji
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

    check_error(argc == 4, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "Nepostojeci fajl");
    check_error(S_ISREG(fInfo.st_mode), "Nije regularni fajl");

    unsigned pozicija = atoi(argv[2]);
    unsigned duzina = atoi(argv[3]);

    int fd = open(argv[1], O_RDONLY);
    check_error(fd != -1, "open1");

    char* buffer = malloc(sizeof(char)*pozicija*2);
    check_error(buffer != NULL, "malloc");
    int readbytes = read(fd, buffer, pozicija + duzina);

    char* niska = malloc(sizeof(char)*duzina + 1);
    check_error(niska != NULL, "malloc");
    for(int i = 0; i < duzina; i++){
        niska[i] = buffer[pozicija + duzina - i - 1];
    }
    niska[duzina] = '\0';

    close(fd);

    fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "open2");
    readbytes = read(fd, buffer, pozicija);
    check_error(readbytes != -1, "read");
    int writebytes = write(fd, niska, duzina);
    check_error(writebytes != -1, "write");

    close(fd);
    free(niska);
    free(buffer);

    exit(EXIT_SUCCESS);
}
