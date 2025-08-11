// Program sa date fajl adrese cita, pise ili dodaje, u slucaju pisanja, ako fajl
// ne postoji, napraviti ga
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

#define MAX_SIZE 1024

void osOperacije(const char* operacija, const char* fPath){

    if(strcmp("-r", operacija) && strcmp("-w", operacija) && strcmp("-a", operacija)){
        check_error(false, "Neispravna operacija\n");
    }

    int fd;
    char buffer[MAX_SIZE];
    long long readBytes;
    if(!strcmp("-r", operacija)){
        fd = open(fPath, O_RDONLY);
        check_error(fd != -1, "open");
        while((readBytes = read(fd, buffer, MAX_SIZE)) > 0){
            if(write(STDOUT_FILENO, buffer, readBytes) < 0){
                check_error(false, "write");
            }
        }
    }
    else if(!strcmp("-w", operacija)){
        fd = open(fPath, O_WRONLY | O_CREAT | O_TRUNC);
        check_error(fd != -1, "open");
        while((readBytes = read(STDIN_FILENO, buffer, MAX_SIZE)) > 0){
            if(write(fd, buffer, readBytes) < 0){
                check_error(false, "write");
            }
        }
    }
    else{
        fd = open(fPath, O_WRONLY | O_APPEND);
        check_error(fd != -1, "open");
        while((readBytes = read(STDIN_FILENO, buffer, MAX_SIZE)) > 0){
            if(write(fd, buffer, readBytes) < 0){
                check_error(false, "write");
            }
        }
    }

    close(fd);
}

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti\n");

    osOperacije(argv[1], argv[2]);

    exit(EXIT_SUCCESS);
}
