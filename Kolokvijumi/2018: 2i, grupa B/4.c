// Program ispisuje tekst od pocizije a duzine b
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

#define MAX_SIZE 1024

int main(int argc, char** argv){

    check_error(argc == 4, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "Nepostojeci fajl");
    check_error(S_ISREG(fInfo.st_mode), "Pogresan tip");

    unsigned pocetak = atoi(argv[2]);
    unsigned duzina = atoi(argv[3]);

    int fd = open(argv[1], O_RDONLY);
    check_error(fd != -1, "open");

    char buffer[MAX_SIZE];
    char* niska = malloc(sizeof(char)*duzina + 1);
    check_error(niska != NULL, "malloc");

    check_error(read(fd, buffer, pocetak + duzina) != -1, "read");
    for(int i = 0; i < duzina; i++){
        niska[i] = buffer[pocetak + i];
    }
    niska[duzina + 1] = '\0';
    printf("%s\n", niska);

    free(niska);
    close(fd);

    exit(EXIT_SUCCESS);
}
