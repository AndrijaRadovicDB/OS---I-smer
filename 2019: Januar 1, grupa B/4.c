// Program cita fajl, gde je svaka linija oblika komanda argument. Program
// pokrece svaku liniju u novom dete procesu, proverava exit kod i ako je
// razlicit od 0 roditelj ispisuje ono sto je procitao iz pajpa u errors.txt
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

#define MAX_LINE (255)
#define MAX_SIZE (4094)
#define RD_END (0)
#define WR_END (1)

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    FILE* f = fopen(argv[1], "r");
    check_error(f != NULL, "fopen");
    FILE* err_out = fopen("errors.txt", "w");
    check_error(err_out != NULL, "fopen");

    char command[MAX_LINE];
    char arg[MAX_LINE];
    while(fscanf(f, "%s%s", command, arg) == 2){

        int pipeFds[2];
        check_error(pipe(pipeFds) != -1, "pipe");

        pid_t cldPid = fork();
        check_error(cldPid != -1, "fork");

        if(cldPid == 0){
            close(pipeFds[RD_END]);

            check_error(dup2(pipeFds[WR_END], STDERR_FILENO) != -1, "dup2");
            check_error(execlp(command, command, arg, NULL) != -1, "execlp");

            close(pipeFds[WR_END]);
        }

        close(pipeFds[WR_END]);
        FILE* f1 = fdopen(pipeFds[RD_END], "r");
        check_error(f1 != NULL, "fdopen");

        // Citanje iz stderr u buffer pa onda posle ide proverava da li je greska
        char buffer[MAX_SIZE];
        int i = 0;
        while((buffer[i] = getc(f1)) != EOF){
            i++;
        }
        buffer[i] = '\0';

        fclose(f1);

        int status = 0;
        check_error(wait(&status) != -1, "wait");
        // Ako smo izlasi i ako je status veci od 0 to je neka greska pa je ispisujemo
        if(WIFEXITED(status)){
            if(WEXITSTATUS(status) > 0){
                fprintf(err_out, "%s", buffer);
            }
        }

    }

    fclose(f);
    fclose(err_out);

    exit(EXIT_SUCCESS);
}
