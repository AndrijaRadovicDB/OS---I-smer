// Program ume da pokrene i izvrsi bilo koji program. Program na kraju ispisuje
// broj linija programa ili Neuspeh u slucaju neuspeha.
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

    check_error(argc > 1, "Argumenti");

    char* command = argv[1];
    char** arguments = malloc((argc - 2)*sizeof(char*));
    for(int i = 0; i < argc; i++){
        arguments[i] = argv[i + 1];
    }
    arguments[argc - 1] = NULL;

    int pipeFds[2];
    check_error(pipe(pipeFds) != -1, "pipe");

    pid_t cldPid = fork();
    check_error(cldPid != -1, "fork");

    if(cldPid == 0){ // Dete
        close(pipeFds[RD_END]);

        check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2");
        // execvp kada saljemo 1 argument kao ceo niz
        check_error(execvp(command, arguments) != -1, "execvp");

    }

    close(pipeFds[WR_END]);
    FILE* f = fdopen(pipeFds[RD_END], "r");
    check_error(f != NULL, "fdopen");

    char* line = NULL;
    size_t read_bytes = 0;
    int line_num = 0;
    while(getline(&line, &read_bytes, f) != -1){
        line_num++;
    }

    int status = 0;
    check_error(waitpid(cldPid, &status, 0) != -1, "waitpid");
    if(!(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)){
        printf("Neuspeh\n");
        exit(EXIT_FAILURE);
    }

    printf("%d\n", line_num);

    fclose(f);
    free(arguments);
    free(line);

    exit(EXIT_SUCCESS);
}
