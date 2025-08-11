// Program cita fajl koji u svakoj liniji ima komadnu i argument. Program pokrece
// svaku komandu u novom detetu, dohvata izalz i racuna ukupan broj bajtova
// koje je procitao za datu komandu, ispisati komandu sa najvecim izlazom
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/wait.h>

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

#define MAX_SIZE (255)
#define RD_END (0)
#define WR_END (1)

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    FILE* f = fopen(argv[1], "r");
    check_error(f != NULL, "fopen");

    char command[MAX_SIZE], arg[MAX_SIZE];
    char max_com[MAX_SIZE], max_arg[MAX_SIZE];
    int max_read_bytes = -1;
    while(fscanf(f, "%s%s", command, arg) == 2){

        int pipeFds[2];
        check_error(pipe(pipeFds) != -1, "pipe");

        pid_t cldPid = fork();
        check_error(cldPid != -1, "fork");

        if(cldPid == 0){
            close(pipeFds[RD_END]);

            check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2");
            check_error(execlp(command, command, arg, NULL) != -1, "execlp");

            close(pipeFds[WR_END]);
        }

        close(pipeFds[WR_END]);

        int read_bytes = 0, bytes = 0;
        char buffer[MAX_SIZE];
        while((read_bytes = read(pipeFds[RD_END], buffer, MAX_SIZE)) > 0){
            bytes += read_bytes;
        }
        check_error(read_bytes != -1, "read");

        if(bytes > max_read_bytes){
            max_read_bytes = bytes;
            strcpy(max_com, command);
            strcpy(max_arg, arg);
        }

        int status = 0;
        check_error(wait(&status) != -1, "wait");

        close(pipeFds[RD_END]);
    }

    printf("%s %s\n", max_com, max_arg);

    fclose(f);

    exit(EXIT_SUCCESS);
}
