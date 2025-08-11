// Program pokrece dete i u njemu izvrsava pwd. Preusmeriti izaz iz deteta,
// procitati ga u roditeljskom procesu i ispisati duzinu putanje.
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

#define RD_END (0)
#define WR_END (1)

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    int pipeFds[2];
    check_error(pipe(pipeFds) != -1, "pipe");
    pid_t cldPid = fork();
    check_error(cldPid != -1, "fork");

    if(cldPid == 0){
        close(pipeFds[RD_END]);

        check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2");
        check_error(execlp("pwd", "pwd", NULL) != -1, "execlp");

        exit(EXIT_SUCCESS);
    }

    close(pipeFds[WR_END]);
    FILE* f = fdopen(pipeFds[RD_END], "r");
    check_error(f != NULL, "fdopen");

    char* line = NULL;
    size_t len = 0;
    check_error(getline(&line, &len, f) != -1, "getline");
    printf("%ld\n", strlen(line));

    int status = 0;
    check_error(wait(&status) != -1, "wait");

    fclose(f);

    exit(EXIT_SUCCESS);
}
