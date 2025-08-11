// Program ispisuje tekst koji se nalazi na odredjenoj poziciji
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    check_error(S_ISREG(fInfo.st_mode), "Nije regularni fajl");

    int fd = open(argv[1], O_RDONLY);
    check_error(fd != -1, "open");
    unsigned offset = atoi(argv[2]);
    unsigned duzina = atoi(argv[3]);

    char* niska = malloc(sizeof(char)*duzina + 1);
    check_error(niska != NULL, "malloc");

    char buffer[MAX_SIZE];
    int bytes_read = read(fd, buffer, offset + duzina);
    check_error(bytes_read != -1, "read");
    int k = 0;
    for(int i = offset; i < offset + duzina; i++){
        niska[k] = buffer[i];
        k++;
    }
    niska[k+1] = '\0';

    printf("%s\n", niska);

    free(niska);
    close(fd);

    exit(EXIT_SUCCESS);
}
