// Program u dete procesu pokrece stat fPath i preusmerava izlaz u roditelja koji
// ispisuje string koji odgovara pravima pristupa i tipu fajla. Ako stat ne uspe
// ispisati iz roditelja neuspeh.
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

    check_error(argc == 2, "Argumenti");

    int pipeFds[2];
    check_error(pipe(pipeFds) != -1, "pipe");

    pid_t cldPid = fork();
    check_error(cldPid != -1, "fork");

    if(cldPid == 0){
        close(pipeFds[RD_END]);

        check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2");
        // --format=%A ispisuje prava pristupa i tip fajla
        check_error(execlp("stat", "stat", "--format=%A", argv[1], NULL) != -1, "execlp");
        check_error(execlp("stat", "stat", argv[1], NULL) != -1, "execlp");

        close(pipeFds[WR_END]);
    }

    close(pipeFds[WR_END]);
    FILE* f = fdopen(pipeFds[RD_END], "r");
    check_error(f != NULL, "fdopen");

    // Ako ne koristimo format
    //char *tmp;
    //sscanf(line, "%s%*s%*s...", tmp);

    int status = 0;
    check_error(wait(&status) != -1, "wait");
    if(!(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)){
        printf("neuspeh\n");
        exit(EXIT_SUCCESS);
    }

    char* line = NULL;
    size_t read_bytes = 0;
    check_error(getline(&line, &read_bytes, f) != -1, "getline");
    printf("%s", line);

    free(line);
    fclose(f);
    close(pipeFds[RD_END]);

    exit(EXIT_SUCCESS);
}
