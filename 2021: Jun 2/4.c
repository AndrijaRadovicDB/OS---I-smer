// Program cita liniju po liniju oblika komanda argument. Svaku liniju pokrece u
// dete procesu i racuna ukupan broj bajtova koje je vratila komanda. Ispisati iz
// roditelja komadnu koja je proizvela najveci izlaz
#define _XOPEN_SOURCE 700
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

#define MAX_SIZE 256
#define RD_END (0)
#define WR_END (1)

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    FILE* f = fopen(argv[1], "r");
    check_error(f != NULL, "fopen");

    char* line = NULL;
    size_t len = 0;
    char max_command[MAX_SIZE];
    char max_arg[MAX_SIZE];
    int max_bytes = -1;
    // I ovo radi uz male izmene tela
    // while(fscanf(f, "%s%s", command, arg) == 2){
    while(getline(&line, &len, f) > 0){

        char command[MAX_SIZE];
        char arg[MAX_SIZE];
        sscanf(line, "%s%s", command, arg);

        int pipeFds[2];
        check_error(pipe(pipeFds) != -1, "pipe");

        pid_t cldPid = fork();
        check_error(cldPid != -1, "fork");

        if(cldPid == 0){
            close(pipeFds[RD_END]);

            check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2");
            check_error(execlp(command, command, arg, NULL) != -1, "execlp");

            exit(EXIT_SUCCESS);
        }

        close(pipeFds[WR_END]);

        char buffer[MAX_SIZE];
        int read_bytes = 0, bytes = 0;
        while((read_bytes = read(pipeFds[RD_END], buffer, MAX_SIZE)) > 0){
            bytes += read_bytes;
        }
        check_error(read_bytes != -1, "read");

        if(bytes > max_bytes){
            max_bytes = bytes;
            strcpy(max_command, command);
            strcpy(max_arg, arg);
        }

        int status = 0;
        check_error(wait(&status) != -1, "status");
    }

    printf("%s %s\n", max_command, max_arg);

    free(line);
    fclose(f);

    exit(EXIT_SUCCESS);
}
