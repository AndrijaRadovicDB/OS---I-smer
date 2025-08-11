// Program upisuje tekst od odredjene pozicije
#define _XOPEN_SOURCE 700
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

    check_error(argc == 3, "Argumenti");

    struct stat fInfo1, fInfo2;
    check_error(stat(argv[1], &fInfo1) != -1, "Nepostojeci fajl");
    check_error(stat(argv[2], &fInfo2) != -1, "Nepostojeci fajl");

    FILE* f = fopen(argv[1], "r");
    check_error(f != NULL, "fopen");

    unsigned pocetak;
    char* rec = malloc(sizeof(char)*MAX_SIZE);
    check_error(rec != NULL, "malloc");
    char* buffer = malloc(sizeof(char)*MAX_SIZE*4);
    check_error(buffer != NULL, "malloc");

    while(fscanf(f, "%u%s", &pocetak, rec) == 2){
        int fd = open(argv[2], O_RDWR);
        check_error(fd != -1, "open");
        check_error(read(fd, buffer, pocetak) != -1, "read");
        check_error(write(fd, rec, strlen(rec)) != -1, "write");
        close(fd);
    }

    fclose(f);
    free(rec);
    free(buffer);

    exit(EXIT_SUCCESS);
}
