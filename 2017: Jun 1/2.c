// Program pokrece dete proces u njemu radi ls -l, preusmeriti ispis na roditelja,
// ako komanda ne uspe ispisati neuspeh
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/wait.h>

#include <stdio.h>
#include <stdlib.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

#define RD_END (0)
#define WR_END (1)
#define MAX_SIZE 8192

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    int pipeFds[2];
    check_error(pipe(pipeFds) != -1, "pipe");

    pid_t childPid = fork();
    check_error(childPid != -1, "fork");

    if(childPid == 0){ // Dete

        close(pipeFds[RD_END]);

        dup2(pipeFds[WR_END], STDOUT_FILENO);

        check_error(execlp("ls", "ls", "-l", argv[1], NULL) != -1, "execlp");
    }

    close(pipeFds[WR_END]);
    char buffer[MAX_SIZE];
    check_error(read(pipeFds[RD_END], buffer, MAX_SIZE) != -1, "read");

    int status = 0;
    check_error(waitpid(childPid, &status, 0) != -1, "waitpid");

    if(WIFEXITED(status)){
        if(WEXITSTATUS(status) != EXIT_SUCCESS){
            printf("Neuspeh\n");
        }
    }
    else{
        printf("Neuspeh\n");
    }

    char niska[MAX_SIZE];
    sscanf(buffer, "%s", niska);
    printf("%s\n", niska);

    exit(EXIT_SUCCESS);
}
