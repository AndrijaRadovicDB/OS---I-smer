// Program cita sadrzaj fajla liniju po liniju, svaka linija je oblika: komanda
// argument. Pokrece se svaka linija u novom detetu, preusmerava se na stdout i
// na kraju ispisuje komandu koja je proizvela najveci izlaz, tj najvise bajtova
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

#define MAX_SIZE (256)
#define RD_END (0)
#define WR_END (1)

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    FILE* f = fopen(argv[1], "r");
    check_error(f != NULL, "fopen");

    int max_bytes = -1;
    char command[MAX_SIZE], arg[MAX_SIZE];
    char max_command[MAX_SIZE], max_arg[MAX_SIZE];

    while(fscanf(f, "%s%s", command, arg) == 2){

        int pipeFds[2];
        check_error(pipe(pipeFds) != -1, "pipe");

        pid_t cldPid = fork();
        check_error(cldPid != -1, "fork");

        if(cldPid == 0){

            close(pipeFds[RD_END]);

            check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2");
            check_error(execlp(command, command, arg, NULL) != -1, "execlp");

        }

        close(pipeFds[WR_END]);

        char buffer[MAX_SIZE];
        int read_bytes = 0;
        int bytes = 0;

        while((read_bytes = read(pipeFds[RD_END], buffer, MAX_SIZE)) > 0){
            bytes += read_bytes;
        }
        if(bytes > max_bytes){
            max_bytes = bytes;
            strcpy(max_command, command);
            strcpy(max_arg, arg);
        }

        int status = 0;
        check_error(wait(&status) != -1, "wait");

    }

    printf("%s %s\n", max_command, max_arg);

    fclose(f);

    exit(EXIT_SUCCESS);
}
