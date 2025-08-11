// Program cita fajl liniju po liniju, linija oblika komanda argument. Svaku
// liniju pokrece u novom dete procesu, u errors.txt roditelj ispisuje rezultat
// komande ako je njen exit kod razlicit od 0.
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

#define MAX_SIZE 255
#define MAX_LINE 4094
#define RD_END (0)
#define WR_END (1)

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    FILE* f = fopen(argv[1], "r");
    check_error(f != NULL, "fopen");
    FILE* errors = fopen("errors.txt", "w");
    check_error(errors != NULL, "fopen");

    char* line = NULL;
    size_t len = 0;
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

            check_error(dup2(pipeFds[WR_END], STDERR_FILENO) != -1, "dup2");
            check_error(execlp(command, command, arg, NULL) != -1, "execlp");

            exit(EXIT_SUCCESS);
        }

        close(pipeFds[WR_END]);
        FILE* f_err = fdopen(pipeFds[RD_END], "r");
        check_error(f_err != NULL, "fdopen");

        // Moze da se uzme da se cita rec po rec i nadovezuje, onda bi imali
        // fscanf i strcat
        char buffer[MAX_LINE];
        char c;
        int i = 0;
        while((c = fgetc(f_err)) != EOF){
            buffer[i] = c;
            i++;
        }
        buffer[i] = '\0';

        int status = 0;
        check_error(wait(&status) != -1, "wait");
        if(!(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)){
            fprintf(errors, "%s\n", buffer);
        }

        fclose(f_err);
    }

    free(line);
    fclose(errors);
    fclose(f);

    exit(EXIT_SUCCESS);
}
