// Program prosledjuje naziv direktorijuma detetu, dete pokusava da napravi dir
// sa svim dozvolama i ekstenzijom .dir. Ako dete zavrsi sa exit kodom razlicitim
// od 0, roditelj treba da zavrsi sa tim istim exit kodom.
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

    check_error(argc == 1, "Argumenti");

    char tmp[MAX_SIZE];
    scanf("%s", tmp);

    char* name = malloc((strlen(tmp) + strlen(".dir") + 1)*sizeof(char));
    check_error(name != NULL, "malloc");
    strcat(name, tmp);
    strcat(name, ".dir");

    int pipeFds[2];
    check_error(pipe(pipeFds) != -1, "pipe");
    pid_t cldPid = fork();
    check_error(cldPid != -1, "fork");

    if(cldPid == 0){
        close(pipeFds[RD_END]);

        check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2");
        mode_t old_umask = umask(0);
        check_error(mkdir(name, 0777) != -1, "mkdir");
        umask(old_umask);

        exit(EXIT_SUCCESS);
    }

    close(pipeFds[WR_END]);

    int status = 0;
    check_error(wait(&status) != -1, "wait");
    if(!(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)){
        exit(WEXITSTATUS(status));
    }

    free(name);

    exit(EXIT_SUCCESS);
}
