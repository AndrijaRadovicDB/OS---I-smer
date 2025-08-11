// Program na datim pozicija date duzine menja sadrzaj
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

    check_error(argc == 5, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "Nepostojeci fajl");
    check_error(S_ISREG(fInfo.st_mode), "Pogresan tip");



    unsigned prva_pozicija = atoi(argv[2]);
    unsigned druga_pozicija = atoi(argv[3]);
    if(prva_pozicija > druga_pozicija){
        unsigned tmp = prva_pozicija;
        prva_pozicija = druga_pozicija;
        druga_pozicija = tmp;
    }
    unsigned duzina = atoi(argv[4]);

    int fd = open(argv[1], O_RDONLY);
    check_error(fd != -1, "open1");

    char* niska1 = malloc(sizeof(char)*duzina + 1);
    check_error(niska1 != NULL, "malloc1");
    char* niska2 = malloc(sizeof(char)*duzina + 1);
    check_error(niska2 != NULL, "malloc2");
    char *buffer = malloc(sizeof(char)*druga_pozicija*2);
    check_error(buffer != NULL, "malloc3");

    int procitano = read(fd, buffer, prva_pozicija + duzina);
    check_error(procitano != -1, "read1");
    for(int i = 0; i < duzina; i++){
        niska1[i] = buffer[prva_pozicija + i];
    }
    niska1[duzina] = '\0';

    close(fd);

    fd = open(argv[1], O_RDONLY);
    check_error(fd != -1, "open2");

    procitano = read(fd, buffer, druga_pozicija + duzina);
    check_error(procitano != -1, "read2");
    for(int i = 0; i < duzina; i++){
        niska2[i] = buffer[druga_pozicija + i];
    }
    niska2[duzina] = '\0';

    close(fd);

    fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "open3");

    procitano = read(fd, buffer, prva_pozicija);
    check_error(procitano != -1, "read3");
    int upisano = write(fd, niska2, duzina);
    check_error(upisano != -1, "write1");

    close(fd);

    fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "open4");

    procitano = read(fd, buffer, druga_pozicija);
    check_error(procitano != -1, "read4");
    upisano = write(fd, niska1, duzina);
    check_error(upisano != -1, "write2");

    close(fd);

    free(niska1);
    free(niska2);
    free(buffer);

    exit(EXIT_SUCCESS);
}
